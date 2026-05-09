#include "AnimationSystem.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include <d3dx12.h>

namespace Engine::ECS {

void AnimationSystem::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    boneMatrixSB_ = std::make_unique<Graphics::StructuredBuffer>();
    boneMatrixSB_->Create(device, sizeof(Engine::GeneratedSchema::BoneData), kMaxBones, nullptr, false); // isUAV = false

    skinnedVertexBuffer_ = std::make_unique<Graphics::StructuredBuffer>();
    skinnedVertexBuffer_->Create(device, sizeof(Asset::Vertex), kMaxSkinnedVertices, nullptr, true); // isUAV = true (Compute Output)

    for (int i = 0; i < kMaxBatches; ++i) {
        skinningParamsCBs_[i] = std::make_unique<Graphics::ConstantBuffer>();
        skinningParamsCBs_[i]->Create(device, sizeof(Engine::GeneratedSchema::SkinningParams));
    }
}

void AnimationSystem::Shutdown() {
    boneMatrixSB_.reset();
    skinnedVertexBuffer_.reset();
    for (int i = 0; i < kMaxBatches; ++i) {
        skinningParamsCBs_[i].reset();
    }
}

void AnimationSystem::Update(Registry& registry, ID3D12GraphicsCommandList* commandList) {
    auto view = registry.GetView<Transform, SkinnedMeshRenderer>();

    auto& sm = Graphics::ShaderManager::GetInstance();
    auto& am = Asset::AssetManager::GetInstance();
    auto& gp = Graphics::GeometryPool::GetInstance();

    auto* pso = sm.GetComputePSO("Skinning");
    auto* rootSig = sm.GetRootSignature("Skinning");

    if (!pso || !rootSig) return;

    // 1. ボーンデータの集計とアップロード
    static std::vector<GeneratedSchema::BoneData> boneData(kMaxBones);
    static float accumulatedTime = 0;
    accumulatedTime += 0.016f;

    // boneData をリセット
    std::fill(boneData.begin(), boneData.end(), GeneratedSchema::BoneData{});

    view.Each([&](Entity entity, Transform& transform, SkinnedMeshRenderer& smr) {
        const auto& meshes = am.GetMeshesByIndex(smr.modelIndex);
        if (meshes.empty()) return;

        const std::vector<Asset::Mesh::Bone>* pMeshBones = nullptr;
        for (const auto& m : meshes) {
            if (m->IsSkinned()) {
                pMeshBones = &m->GetBones();
                break;
            }
        }
        
        if (!pMeshBones) return;

        for (uint32_t i = 0; i < (uint32_t)pMeshBones->size(); ++i) {
            GeneratedSchema::BoneData data;
            float angle = std::sin(accumulatedTime * 4.0f + i * 0.5f) * 1.0f;
            Math::Matrix4x4 anim = Math::Matrix4x4::MakeRotateZ(angle);
            Math::Matrix4x4 finalMatrix = (*pMeshBones)[i].offsetMatrix * anim;
            data.transform = finalMatrix.Transpose();
            
            if (smr.skeletonIndex + i < kMaxBones) {
                boneData[smr.skeletonIndex + i] = data;
            }
        }
    });
    
    boneMatrixSB_->Update(boneData.data(), (uint32_t)(boneData.size() * sizeof(GeneratedSchema::BoneData)));

    // 2. スキニングの実行
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(skinnedVertexBuffer_->GetResource(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        commandList->ResourceBarrier(1, &barrier);
    }

    commandList->SetComputeRootSignature(rootSig->Get());
    commandList->SetPipelineState(pso);

    uint32_t cbIndex = 0;
    uint32_t vertexStackPtr = 0;

    view.Each([&](Entity entity, Transform& transform, SkinnedMeshRenderer& smr) {
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
                setParam("gBones", boneMatrixSB_->GetResource()->GetGPUVirtualAddress(), false, true);
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
