#include "Renderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/GPUCullingManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/MaterialManager.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Asset/Mesh.h"
#include "Engine/Asset/Texture.h"
#include "Engine/Common/Console.h"
#include <algorithm>
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Resource/GeometryPool.h"

namespace Engine::Graphics {

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
    if (static_cast<uint32_t>(queue_.size()) < kMaxInstances) {
        queue_.push_back(request);
    }
}

void Renderer::ClearQueue() {
    queue_.clear();
}

void Renderer::Extract() {
    if (queue_.empty()) return;

    auto& graphicsEngine = GraphicsEngine::GetInstance();
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& assetManager = Asset::AssetManager::GetInstance();

    // 中央管理のフレームインデックスを取得
    uint32_t frameIndex = graphicsEngine.GetCurrentFrameIndex();

    // 1. ソートしてバッチングしやすくする (インデックス比較なので高速)
    std::sort(queue_.begin(), queue_.end(), [](const RenderRequest& a, const RenderRequest& b) {
        if (a.modelIndex != b.modelIndex) return a.modelIndex < b.modelIndex;
        return a.materialIndex < b.materialIndex;
    });

    // 2. 全インスタンスデータの抽出
    std::vector<GeneratedSchema::InstanceData> instanceData;
    instanceData.reserve(queue_.size());
    for (const auto& req : queue_) {
        GeneratedSchema::InstanceData data;
        data.world = req.world;
        data.entityID = req.entityID;
        data.postProcessFlags = req.postProcessFlags;
        data.modelIndex = req.modelIndex;

        const auto& meshes = assetManager.GetMeshesByIndex(req.modelIndex);
        if (!meshes.empty()) {
            const auto& min3 = meshes[0]->GetAABBMin();
            const auto& max3 = meshes[0]->GetAABBMax();
            data.aabbMin = { min3.x, min3.y, min3.z, 1.0f };
            data.aabbMax = { max3.x, max3.y, max3.z, 1.0f };
        }
        
        auto* mat = materialManager.GetMaterialByIndex(req.materialIndex);
        if (mat) {
            data.baseColor = mat->baseColor;
            data.textureIndex = (mat->textureIndex != 0xFFFFFFFF) ? mat->textureIndex : 0;
        } else {
            data.baseColor = { 1, 1, 1, 1 };
            data.textureIndex = 0;
        }
        instanceData.push_back(data);
    }

    // 3. GPUバッファへのアップロード（frameIndex を使用）
    instanceSBs_[frameIndex]->Update(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(GeneratedSchema::InstanceData)));
}

void Renderer::RenderZPrepass(const RenderContext& context) {
    if (context.cullingManager) context.cullingManager->ResetBatchIndex(); // インデックスリセット
    PipelineStateDesc desc;
    desc.usePS = false;
    desc.numRenderTargets = 0;
    desc.depthWriteEnable = true;
    desc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
    RenderInternal(context, desc);
}

void Renderer::Render(const RenderContext& context) {
    if (context.cullingManager) context.cullingManager->ResetBatchIndex(); // インデックスリセット
    PipelineStateDesc desc;
    desc.usePS = true;
    desc.numRenderTargets = context.numRenderTargets;
    for (uint32_t i = 0; i < context.numRenderTargets; ++i) {
        desc.rtvFormats[i] = context.rtvFormats[i];
    }
    desc.depthWriteEnable = false;
    desc.depthFunc = D3D12_COMPARISON_FUNC_EQUAL;
    RenderInternal(context, desc);
}

void Renderer::RenderInternal(const RenderContext& context, const PipelineStateDesc& baseDesc) {
    if (queue_.empty()) return;

    auto& assetManager = Asset::AssetManager::GetInstance();
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& textureManager = Asset::TextureManager::GetInstance();
    auto& shaderManager = ShaderManager::GetInstance();
    auto& geoPool = GeometryPool::GetInstance();
    auto& graphics = GraphicsEngine::GetInstance();

    ID3D12GraphicsCommandList* commandList = context.commandList;

    // バッチ処理ループ
    uint32_t currentInstanceStart = 0;
    uint32_t batchIndex = 0;
    const uint32_t totalInstances = static_cast<uint32_t>(queue_.size());

    while (currentInstanceStart < totalInstances && batchIndex < 64 /* GPUCullingManager::kMaxBatches */) {
        const auto& batchStartReq = queue_[currentInstanceStart];
        
        uint32_t batchSize = 0;
        for (uint32_t i = currentInstanceStart; i < totalInstances; ++i) {
            if (queue_[i].modelIndex == batchStartReq.modelIndex && 
                queue_[i].materialIndex == batchStartReq.materialIndex) {
                batchSize++;
            } else {
                break;
            }
        }

        auto* mat = materialManager.GetMaterialByIndex(batchStartReq.materialIndex);
        if (mat) {
            auto* pso = shaderManager.GetOrCreatePSO(mat->pipelineName, baseDesc);
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            
            // 1. このバッチ専用に GPU カリングを実行
            if (context.cullingManager) {
                context.cullingManager->Execute(
                    commandList,
                    context.sceneCBAddress,
                    instanceSBs_[context.frameIndex].get(),
                    batchSize,
                    context.viewProj,
                    context.meshInfoBufferAddress,
                    batchStartReq.modelIndex,
                    currentInstanceStart,
                    batchIndex
                );
            }

            // 2. 描画設定
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
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
                commandList->SetGraphicsRootDescriptorTable(texIdx, textureManager.GetSrvHeap()->GetGPUHandle(0));

            // GPUカリング後のバッファをバインド
            if (context.cullingManager) {
                D3D12_GPU_VIRTUAL_ADDRESS culledAddr = context.cullingManager->GetOutputInstances()->GetGPUVirtualAddress();
                culledAddr += static_cast<UINT64>(batchIndex * 2048) * sizeof(GeneratedSchema::InstanceData);
                setSRV("gInstances", culledAddr);
            }

            setSRV("gPointLights", context.pointLightBufferAddress);
            setSRV("gLightGrid", context.lightGridBufferAddress);
            setSRV("gLightIndexList", context.lightIndexListBufferAddress);
            setSRV("gVertices", geoPool.GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());

            D3D12_INDEX_BUFFER_VIEW ibv = geoPool.GetIndexBuffer()->GetView();
            commandList->IASetIndexBuffer(&ibv);
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // 3. ExecuteIndirect の実行
            if (context.cullingManager && context.cullingManager->GetCommandSignature()) {
                ID3D12Resource* cmdBuf = context.cullingManager->GetIndirectCommandBuffer()->GetResource();
                ID3D12Resource* cntBuf = context.cullingManager->GetDrawArgsBuffer()->GetResource();
                
                commandList->ExecuteIndirect(
                    context.cullingManager->GetCommandSignature(),
                    2048, 
                    cmdBuf,
                    static_cast<UINT64>(batchIndex * 2048) * sizeof(uint32_t) * 5,
                    cntBuf,
                    static_cast<UINT64>(batchIndex) * sizeof(uint32_t)
                );
            } else {
                const auto& meshes = assetManager.GetMeshesByIndex(batchStartReq.modelIndex);
                for (const auto& mesh : meshes) {
                    mesh->Draw(commandList, batchSize);
                }
            }
        }

        currentInstanceStart += batchSize;
        batchIndex++;
    }
}

} // namespace Engine::Graphics
