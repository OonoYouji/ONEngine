#include "Renderer.h"
#include <algorithm>
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Resource/GeometryPool.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Graphics/Core/GPUCullingManager.h"
#include "Engine/ECS/Systems/AnimationSystem.h"

namespace Engine::Graphics {

Renderer* Renderer::instance_ = nullptr;

void Renderer::Initialize(RenderDevice* device) {
    device_ = device;
    for (uint32_t i = 0; i < kBufferCount; ++i) {
        instanceSBs_[i] = std::make_unique<StructuredBuffer>();
        instanceSBs_[i]->Create(device, sizeof(GeneratedSchema::InstanceData), kMaxInstances);
    }
}

void Renderer::Shutdown() {
    for (uint32_t i = 0; i < kBufferCount; ++i) {
        instanceSBs_[i].reset();
    }
}

void Renderer::PushRequest(const RenderRequest& request) {
    if (queue_.size() >= kMaxInstances) return;
    queue_.push_back(request);
}

void Renderer::ClearQueue() {
    queue_.clear();
}

void Renderer::Extract() {
    if (queue_.empty()) return;

    auto& graphicsEngine = GraphicsEngine::GetInstance();
    uint32_t frameIndex = graphicsEngine.GetCurrentFrameIndex();
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& assetManager = Asset::AssetManager::GetInstance();

    uint32_t instanceCount = static_cast<uint32_t>((std::min)(queue_.size(), static_cast<size_t>(kMaxInstances)));
    std::vector<GeneratedSchema::InstanceData> instances;
    instances.reserve(instanceCount);

    for (uint32_t i = 0; i < instanceCount; ++i) {
        const auto& req = queue_[i];
        GeneratedSchema::InstanceData data{}; // ゼロ初期化
        data.world = req.world;
        data.entityID = req.entityID;
        data.postProcessFlags = req.postProcessFlags;
        data.vertexOffset = req.vertexOffset;
        data.modelIndex = req.modelIndex;
        
        // マテリアル情報の取得
        auto* mat = materialManager.GetMaterialByIndex(req.materialIndex);
        if (mat) {
            data.textureIndex = mat->textureIndex;
            data.baseColor = mat->baseColor;
        } else {
            data.textureIndex = 0; // デフォルト
            data.baseColor = { 1, 1, 1, 1 };
        }

        // AABBの取得 (カリング用)
        const auto& meshes = assetManager.GetMeshesByIndex(req.modelIndex);
        if (req.subMeshIndex < meshes.size()) {
            data.aabbMin = { meshes[req.subMeshIndex]->GetAABBMin().x, meshes[req.subMeshIndex]->GetAABBMin().y, meshes[req.subMeshIndex]->GetAABBMin().z, 1.0f };
            data.aabbMax = { meshes[req.subMeshIndex]->GetAABBMax().x, meshes[req.subMeshIndex]->GetAABBMax().y, meshes[req.subMeshIndex]->GetAABBMax().z, 1.0f };
        }

        instances.push_back(data);
    }

    instanceSBs_[frameIndex]->Update(instances.data(), static_cast<uint32_t>(instances.size() * sizeof(GeneratedSchema::InstanceData)));
}

void Renderer::RenderZPrepass(const RenderContext& context) {
    // Z-Prepass (実装省略)
}

void Renderer::Render(const RenderContext& context) {
    static uint32_t frameCount = 0;
    frameCount++;

    if (queue_.empty()) {
        if (frameCount % 100 == 0) {
            Engine::Console::Log("Renderer: Render called with empty queue.");
        }
        return;
    }

    uint32_t totalInstances = static_cast<uint32_t>(queue_.size());
    if (frameCount % 100 == 0) {
        Engine::Console::Log(std::format("Renderer: Rendering {} instances.", totalInstances));
    }
    
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& assetManager = Asset::AssetManager::GetInstance();
    auto& shaderManager = ShaderManager::GetInstance();
    auto& geoPool = GeometryPool::GetInstance();
    auto& graphics = GraphicsEngine::GetInstance();

    uint32_t currentInstanceStart = 0;
    uint32_t batchIndex = 0;
    const uint32_t kMaxBatches = 64; 

    if (context.cullingManager) {
        context.cullingManager->ResetBatchIndex();
    }

    Graphics::PipelineStateDesc baseDesc;
    baseDesc.numRenderTargets = context.numRenderTargets;
    for (uint32_t i = 0; i < context.numRenderTargets; ++i) {
        baseDesc.rtvFormats[i] = context.rtvFormats[i];
    }

    auto* commandList = context.commandList;

    while (currentInstanceStart < totalInstances) {
        if (batchIndex >= kMaxBatches) {
            Engine::Console::LogError(std::format("Renderer: Batch count exceeded kMaxBatches ({}).", kMaxBatches));
            break;
        }

        const auto& batchStartReq = queue_[currentInstanceStart];
        uint32_t batchSize = 1;

        for (uint32_t i = currentInstanceStart + 1; i < totalInstances; ++i) {
            if (queue_[i].modelIndex == batchStartReq.modelIndex && 
                queue_[i].materialIndex == batchStartReq.materialIndex &&
                queue_[i].isSkinned == batchStartReq.isSkinned &&
                queue_[i].vertexOffset == batchStartReq.vertexOffset) {
                batchSize++;
            } else {
                break;
            }
        }

        if (frameCount % 100 == 0) {
             Engine::Console::Log(std::format("Renderer: Batch {}: ModelIdx={}, MatIdx={}, Size={}", batchIndex, batchStartReq.modelIndex, batchStartReq.materialIndex, batchSize));
        }

        auto* mat = materialManager.GetMaterialByIndex(batchStartReq.materialIndex);
        if (mat) {
            auto* pso = shaderManager.GetOrCreatePSO(mat->pipelineName, baseDesc);
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            
            if (!pso || !rootSig) {
                currentInstanceStart += batchSize;
                continue;
            }

            if (context.cullingManager) {
                context.cullingManager->Execute(
                    commandList,
                    context.sceneCBAddress,
                    instanceSBs_[context.frameIndex].get(),
                    batchSize,
                    context.viewProj,
                    context.meshInfoBufferAddress,
                    batchStartReq.modelIndex,
                    batchStartReq.subMeshIndex,
                    currentInstanceStart,
                    batchIndex
                );
            }

            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { graphics.GetSRVHeap()->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            auto setCBV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
                auto idx = rootSig->GetParameterIndex(name);
                if (idx != RootSignature::kInvalidIndex) commandList->SetGraphicsRootConstantBufferView(idx, addr);
            };
            auto setSRV = [&](const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS addr) {
                auto idx = rootSig->GetParameterIndex(name);
                if (idx != RootSignature::kInvalidIndex) commandList->SetGraphicsRootShaderResourceView(idx, addr);
            };

            setCBV("gSceneData", context.sceneCBAddress);
            
            auto texIdx = rootSig->GetParameterIndex("gTextures");
            if (texIdx != RootSignature::kInvalidIndex)
                commandList->SetGraphicsRootDescriptorTable(texIdx, graphics.GetSRVHeap()->GetGPUHandle(0));

            if (context.cullingManager) {
                D3D12_GPU_VIRTUAL_ADDRESS culledAddr = context.cullingManager->GetOutputInstances()->GetGPUVirtualAddress();
                culledAddr += static_cast<UINT64>(batchIndex * 2048) * sizeof(GeneratedSchema::InstanceData);
                setSRV("gInstances", culledAddr);
            } else {
                D3D12_GPU_VIRTUAL_ADDRESS addr = instanceSBs_[context.frameIndex]->GetResource()->GetGPUVirtualAddress();
                addr += static_cast<UINT64>(currentInstanceStart) * sizeof(GeneratedSchema::InstanceData);
                setSRV("gInstances", addr);
            }

            setSRV("gPointLights", context.pointLightBufferAddress);
            if (context.lightGridBufferAddress != 0) setSRV("gLightGrid", context.lightGridBufferAddress);
            if (context.lightIndexListBufferAddress != 0) setSRV("gLightIndexList", context.lightIndexListBufferAddress);
            
            if (batchStartReq.isSkinned && context.animationSystem) {
                setSRV("gVertices", context.animationSystem->GetSkinnedVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            } else {
                setSRV("gVertices", geoPool.GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            }

            setSRV("gMeshInfos", context.meshInfoBufferAddress);

            D3D12_INDEX_BUFFER_VIEW ibv = geoPool.GetIndexBuffer()->GetView();
            commandList->IASetIndexBuffer(&ibv);
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            if (context.cullingManager && context.cullingManager->GetCommandSignature()) {
                ID3D12Resource* cmdBuf = context.cullingManager->GetIndirectCommandBuffer()->GetResource();
                commandList->ExecuteIndirect(
                    context.cullingManager->GetCommandSignature(),
                    1,
                    cmdBuf,
                    static_cast<UINT64>(batchIndex) * sizeof(GPUCullingManager::DrawIndexedArguments),
                    nullptr,
                    0
                );
            } else {
                const auto& meshes = assetManager.GetMeshesByIndex(batchStartReq.modelIndex);
                if (batchStartReq.subMeshIndex < meshes.size()) {
                    meshes[batchStartReq.subMeshIndex]->Draw(commandList, batchSize);
                }
            }
        }

        currentInstanceStart += batchSize;
        batchIndex++;
    }
}

} // namespace Engine::Graphics
