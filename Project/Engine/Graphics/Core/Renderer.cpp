#include "Renderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
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
        data.vertexOffset = req.vertexOffset; // 追加
        
        auto* mat = materialManager.GetMaterialByIndex(req.materialIndex);
        if (mat) {
            data.baseColor = mat->baseColor;
            data.textureIndex = (mat->textureIndex != 0xFFFFFFFF) ? mat->textureIndex : 0;
        } else {
            data.baseColor = { 1, 1, 1, 1 };
            data.textureIndex = 0;
        }
        data.padding = { 0, 0 };
        instanceData.push_back(data);
    }

    // 3. GPUバッファへのアップロード（frameIndex を使用）
    instanceSBs_[frameIndex]->Update(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(GeneratedSchema::InstanceData)));
}

void Renderer::RenderZPrepass(const RenderContext& context) {
    PipelineStateDesc desc;
    desc.usePS = false;
    desc.numRenderTargets = 0;
    desc.depthWriteEnable = true;
    desc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
    RenderInternal(context, desc);
}

void Renderer::Render(const RenderContext& context) {
    PipelineStateDesc desc;
    desc.usePS = true;
    desc.numRenderTargets = 1;
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

    ID3D12GraphicsCommandList* commandList = context.commandList;
    auto* currentSB = instanceSBs_[context.frameIndex].get();

    uint32_t currentInstanceStart = 0;
    const uint32_t totalInstances = static_cast<uint32_t>(queue_.size());

    while (currentInstanceStart < totalInstances) {
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
            
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            auto sceneIdx = rootSig->GetParameterIndex("gSceneData");
            if (sceneIdx != RootSignature::kInvalidIndex) {
                commandList->SetGraphicsRootConstantBufferView(sceneIdx, context.sceneCBAddress);
            }

            auto texIdx = rootSig->GetParameterIndex("gTextures");
            if (texIdx != RootSignature::kInvalidIndex) {
                commandList->SetGraphicsRootDescriptorTable(texIdx, textureManager.GetSrvHeap()->GetGPUHandle(0));
            }
            
            auto instIdx = rootSig->GetParameterIndex("gInstances");
            if (instIdx != RootSignature::kInvalidIndex) {
                D3D12_GPU_VIRTUAL_ADDRESS instBufferAddr = currentSB->GetResource()->GetGPUVirtualAddress();
                instBufferAddr += static_cast<UINT64>(currentInstanceStart) * sizeof(GeneratedSchema::InstanceData);
                commandList->SetGraphicsRootShaderResourceView(instIdx, instBufferAddr);
            }

            auto pointLightIdx = rootSig->GetParameterIndex("gPointLights");
            if (pointLightIdx != RootSignature::kInvalidIndex && context.pointLightBufferAddress != 0) {
                commandList->SetGraphicsRootShaderResourceView(pointLightIdx, context.pointLightBufferAddress);
            }

            // --- ジオメトリプールのバインド ---
            auto& geoPool = GeometryPool::GetInstance();
            auto vertIdx = rootSig->GetParameterIndex("gVertices");
            if (vertIdx != RootSignature::kInvalidIndex) {
                commandList->SetGraphicsRootShaderResourceView(vertIdx, geoPool.GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
            }
            D3D12_INDEX_BUFFER_VIEW ibv = geoPool.GetIndexBuffer()->GetView();
            commandList->IASetIndexBuffer(&ibv);

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            const auto& meshes = assetManager.GetMeshesByIndex(batchStartReq.modelIndex);
            for (const auto& mesh : meshes) {
                mesh->Draw(commandList, batchSize);
            }
        }

        currentInstanceStart += batchSize;
    }
}

} // namespace Engine::Graphics
