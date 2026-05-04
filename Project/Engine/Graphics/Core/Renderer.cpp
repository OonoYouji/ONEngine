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
    auto& textureManager = Asset::TextureManager::GetInstance();

    // 中央管理のフレームインデックスを取得
    uint32_t frameIndex = graphicsEngine.GetCurrentFrameIndex();

    // 1. ソートしてバッチングしやすくする
    std::sort(queue_.begin(), queue_.end(), [](const RenderRequest& a, const RenderRequest& b) {
        if (a.modelName != b.modelName) return a.modelName < b.modelName;
        return a.materialName < b.materialName;
    });

    // 2. 全インスタンスデータの抽出
    std::vector<GeneratedSchema::InstanceData> instanceData;
    instanceData.reserve(queue_.size());
    for (const auto& req : queue_) {
        GeneratedSchema::InstanceData data;
        data.world = req.world;
        
        auto* mat = materialManager.GetMaterial(req.materialName);
        if (mat) {
            data.baseColor = mat->baseColor;
            auto* tex = textureManager.GetTexture(mat->textureName);
            if (tex) {
                data.textureIndex = tex->GetIndex();
            } else {
                data.textureIndex = 0;
            }
        } else {
            data.baseColor = { 1, 1, 1, 1 };
            data.textureIndex = 0;
        }
        data._padding[0] = data._padding[1] = data._padding[2] = 0.0f;
        instanceData.push_back(data);
    }

    // 3. GPUバッファへのアップロード（frameIndex を使用）
    instanceSBs_[frameIndex]->Update(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(GeneratedSchema::InstanceData)));
}

void Renderer::RenderZPrepass(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress) {
    PipelineStateDesc desc;
    desc.usePS = false;
    desc.numRenderTargets = 0;
    desc.depthWriteEnable = true;
    desc.depthFunc = D3D12_COMPARISON_FUNC_LESS;
    RenderInternal(commandList, sceneCBAddress, desc);
}

void Renderer::Render(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress) {
    PipelineStateDesc desc;
    desc.usePS = true;
    desc.numRenderTargets = 1;
    desc.depthWriteEnable = false;
    desc.depthFunc = D3D12_COMPARISON_FUNC_EQUAL;
    RenderInternal(commandList, sceneCBAddress, desc);
}

void Renderer::RenderInternal(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress, const PipelineStateDesc& baseDesc) {
    if (queue_.empty()) return;

    auto& graphicsEngine = GraphicsEngine::GetInstance();
    auto& assetManager = Asset::AssetManager::GetInstance();
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& textureManager = Asset::TextureManager::GetInstance();
    auto& shaderManager = ShaderManager::GetInstance();

    uint32_t frameIndex = graphicsEngine.GetCurrentFrameIndex();
    auto* currentSB = instanceSBs_[frameIndex].get();

    uint32_t currentInstanceStart = 0;
    const uint32_t totalInstances = static_cast<uint32_t>(queue_.size());

    while (currentInstanceStart < totalInstances) {
        const auto& batchStartReq = queue_[currentInstanceStart];
        
        uint32_t batchSize = 0;
        for (uint32_t i = currentInstanceStart; i < totalInstances; ++i) {
            if (queue_[i].modelName == batchStartReq.modelName && 
                queue_[i].materialName == batchStartReq.materialName) {
                batchSize++;
            } else {
                break;
            }
        }

        auto* mat = materialManager.GetMaterial(batchStartReq.materialName);
        if (mat) {
            auto* pso = shaderManager.GetOrCreatePSO(mat->pipelineName, baseDesc);
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            auto sceneIdx = rootSig->GetParameterIndex("gSceneData");
            if (sceneIdx != RootSignature::kInvalidIndex) {
                commandList->SetGraphicsRootConstantBufferView(sceneIdx, sceneCBAddress);
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

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            const auto& meshes = assetManager.GetMeshes(batchStartReq.modelName);
            for (const auto& mesh : meshes) {
                auto vertIdx = rootSig->GetParameterIndex("gVertices");
                if (vertIdx != RootSignature::kInvalidIndex) {
                    commandList->SetGraphicsRootShaderResourceView(vertIdx, mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
                }
                D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBuffer()->GetView();
                commandList->IASetIndexBuffer(&ibv);
                
                commandList->DrawIndexedInstanced(
                    static_cast<UINT>(mesh->GetIndexBuffer()->GetCount()), 
                    static_cast<UINT>(batchSize), 
                    0, 0, 0);
            }
        }

        currentInstanceStart += batchSize;
    }
}

} // namespace Engine::Graphics
