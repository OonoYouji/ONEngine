#include "AnimationSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Asset/Model.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Common/Console.h"
#include <d3dx12.h>

namespace Engine::ECS {

namespace {
    template<typename T>
    T Interpolate(const std::vector<Asset::KeyFrame<T>>& keys, float time) {
        if (keys.empty()) return {};
        if (keys.size() == 1 || time <= keys[0].time) return keys[0].value;
        if (time >= keys.back().time) return keys.back().value;

        for (size_t i = 0; i < keys.size() - 1; ++i) {
            if (time < keys[i + 1].time) {
                float t = (time - keys[i].time) / (keys[i + 1].time - keys[i].time);
                if constexpr (std::is_same_v<T, Math::Quaternion>) {
                    return Math::Quaternion::Slerp(keys[i].value, keys[i + 1].value, t);
                } else {
                    return Math::Vector3::Lerp(keys[i].value, keys[i + 1].value, t);
                }
            }
        }
        return keys.back().value;
    }
}

void AnimationSystem::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    for (int i = 0; i < kBufferCount; ++i) {
        boneMatrixSBs_[i] = std::make_unique<Graphics::StructuredBuffer>();
        boneMatrixSBs_[i]->Create(device, sizeof(Engine::GeneratedSchema::BoneData), kMaxBones, nullptr, false);
    }

    skinnedVertexBuffer_ = std::make_unique<Graphics::StructuredBuffer>();
    skinnedVertexBuffer_->Create(device, sizeof(Asset::Vertex), kMaxSkinnedVertices, nullptr, true);

    for (int i = 0; i < kMaxBatches; ++i) {
        skinningParamsCBs_[i] = std::make_unique<Graphics::ConstantBuffer>();
        skinningParamsCBs_[i]->Create(device, sizeof(Engine::GeneratedSchema::SkinningParams));
    }
}

void AnimationSystem::Shutdown() {
    for (int i = 0; i < kBufferCount; ++i) boneMatrixSBs_[i].reset();
    skinnedVertexBuffer_.reset();
    for (int i = 0; i < kMaxBatches; ++i) skinningParamsCBs_[i].reset();
}

void AnimationSystem::Update(Registry& registry, ID3D12GraphicsCommandList* commandList, float dt, uint32_t frameIndex) {
    auto view = registry.GetView<Transform, SkinnedMeshRenderer>();

    auto& sm = Graphics::ShaderManager::GetInstance();
    auto& am = Asset::AssetManager::GetInstance();
    auto& gp = Graphics::GeometryPool::GetInstance();

    auto* pso = sm.GetComputePSO("Skinning");
    auto* rootSig = sm.GetRootSignature("Skinning");

    if (!pso || !rootSig) return;

    auto& currentBoneSB = boneMatrixSBs_[frameIndex];

    // 1. ボーンデータの集計
    static std::vector<GeneratedSchema::BoneData> boneData(kMaxBones);
    accumulatedTime_ += dt;

    // 単位行列で初期化
    for (auto& bd : boneData) {
        bd.transform = Math::Matrix4x4::kIdentity;
    }

    uint32_t updatedEntities = 0;
    uint32_t updatedBones = 0;

    view.Each([&](Entity entity, Transform& /*unused*/, SkinnedMeshRenderer& smr) {
        auto model = am.GetModelByIndex(smr.modelIndex);
        if (!model) return;

        const auto& meshes = model->GetMeshes();
        if (meshes.empty()) return;

        const std::vector<Asset::Mesh::Bone>* pMeshBones = nullptr;
        for (const auto& m : meshes) {
            if (m->IsSkinned()) {
                pMeshBones = &m->GetBones();
                break;
            }
        }
        
        if (!pMeshBones) return;

        updatedEntities++;

        // アニメーションデータの取得
        const Asset::Animation* pAnim = nullptr;
        if (!model->GetAnimations().empty()) {
            pAnim = &model->GetAnimations()[0]; // とりあえず最初のものを再生
        }

        // 階層構造に従ってグローバル行列を計算
        std::vector<Math::Matrix4x4> modelSpaceTransforms(pMeshBones->size());

        for (uint32_t i = 0; i < (uint32_t)pMeshBones->size(); ++i) {
            const auto& bone = (*pMeshBones)[i];
            
            Math::Matrix4x4 localAnimated;
            if (pAnim && pAnim->nodeAnimations.count(bone.name)) {
                const auto& nodeAnim = pAnim->nodeAnimations.at(bone.name);
                float animTime = std::fmod(accumulatedTime_, pAnim->duration);

                // 初期値として T-pose の成分を使用する
                Math::Vector3 pos = bone.localMatrix.ExtractTranslation();
                Math::Quaternion rot = bone.localMatrix.ExtractRotation();
                Math::Vector3 scale = bone.localMatrix.ExtractScale();

                if (!nodeAnim.translate.empty()) pos = Interpolate(nodeAnim.translate, animTime);
                if (!nodeAnim.rotate.empty()) rot = Interpolate(nodeAnim.rotate, animTime);
                if (!nodeAnim.scale.empty()) scale = Interpolate(nodeAnim.scale, animTime);

                // ローカル行列の構築
                localAnimated = Math::Matrix4x4::MakeAffine(scale, rot, pos);
            } else {
                // アニメーションがない場合は T-pose のローカル行列を使用
                localAnimated = bone.localMatrix;
            }

            // 親の行列と結合してモデル空間の行列を求める
            if (bone.parentIndex == -1) {
                modelSpaceTransforms[i] = localAnimated;
            } else {
                modelSpaceTransforms[i] = localAnimated * modelSpaceTransforms[bone.parentIndex];
            }
            
            // 最終的なスキニング行列 = InverseBindPose * AnimatedModelSpaceTransform
            Math::Matrix4x4 finalMatrix = bone.offsetMatrix * modelSpaceTransforms[i];

            if (smr.skeletonIndex + i < kMaxBones) {
                boneData[smr.skeletonIndex + i].transform = finalMatrix;
                updatedBones++;
            }
        }
    });
    
    // 定期的なログ
    static int logCounter = 0;
    if (logCounter++ % 60 == 0 && updatedBones > 0) {
        Engine::Console::Log(std::format("Anim System: Time={:.2f}, Frame={}, Entities={}, Bones={}", accumulatedTime_, frameIndex, updatedEntities, updatedBones));
    }

    currentBoneSB->Update(boneData.data(), (uint32_t)(boneData.size() * sizeof(GeneratedSchema::BoneData)));

    // 2. スキニングの実行
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(skinnedVertexBuffer_->GetResource(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(1, &barrier);
    }

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);

    uint32_t cbIndex = 0;
    uint32_t vertexStackPtr = 0;

    view.Each([&](Entity entity, Transform& /*unused*/, SkinnedMeshRenderer& smr) {
        const auto& meshes = am.GetMeshesByIndex(smr.modelIndex);
        if (meshes.empty()) return;

        for (const auto& meshPtr : meshes) {
            auto* mesh = meshPtr.get();
            uint32_t vCount = mesh->GetVertexCount();

            if (vertexStackPtr + vCount > kMaxSkinnedVertices) break;
            uint32_t allocatedOffset = vertexStackPtr;
            vertexStackPtr += vCount;

            if (meshPtr == meshes[0]) {
                smr.internalVertexOffset = allocatedOffset;
            }

            if (cbIndex < kMaxBatches) {
                GeneratedSchema::SkinningParams params;
                params.vertexCount = vCount;
                params.inputVertexOffset = mesh->GetVertexOffset();
                params.outputVertexOffset = allocatedOffset; 
                params.boneOffset = smr.skeletonIndex;
                params.skinningEnabled = mesh->IsSkinned() ? 1 : 0;

                skinningParamsCBs_[cbIndex]->Update(&params, sizeof(params));

                auto setParam = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr, bool isUAV = false, bool isSRV = false) {
                    auto idx = rootSig->GetParameterIndex(name);
                    if (idx == Graphics::RootSignature::kInvalidIndex) return;
                    if (isUAV) commandList->SetComputeRootUnorderedAccessView(idx, addr);
                    else if (isSRV) commandList->SetComputeRootShaderResourceView(idx, addr);
                    else commandList->SetComputeRootConstantBufferView(idx, addr);
                };

                setParam("gParams", skinningParamsCBs_[cbIndex]->GetGPUVirtualAddress());
                setParam("gBones", currentBoneSB->GetResource()->GetGPUVirtualAddress(), false, true);
                setParam("gInputVertices", gp.GetVertexBuffer()->GetResource()->GetGPUVirtualAddress(), false, true);
                setParam("gBoneWeights", gp.GetBoneWeightBuffer()->GetResource()->GetGPUVirtualAddress(), false, true);
                setParam("gOutputVertices", skinnedVertexBuffer_->GetResource()->GetGPUVirtualAddress(), true);

                commandList->Dispatch((vCount + 63) / 64, 1, 1);
                cbIndex++;
            }
        }
    });

    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(skinnedVertexBuffer_->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);
    }
}

} // namespace Engine::ECS
