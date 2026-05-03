#include "Renderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
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
    if (queue_.size() < kMaxInstances) {
        queue_.push_back(request);
    }
}

void Renderer::ClearQueue() {
    queue_.clear();
}

void Renderer::Extract() {
    if (queue_.empty()) return;

    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& textureManager = Asset::TextureManager::GetInstance();

    // 1. フレームインデックスの更新
    currentFrameIndex_ = (currentFrameIndex_ + 1) % kBufferCount;

    // 2. ソートしてバッチングしやすくする（Model -> Material順）
    std::sort(queue_.begin(), queue_.end(), [](const RenderRequest& a, const RenderRequest& b) {
        if (a.modelName != b.modelName) return a.modelName < b.modelName;
        return a.materialName < b.materialName;
    });

    // 3. 全インスタンスデータの抽出
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
        instanceData.push_back(data);
    }

    // 4. GPUバッファへのアップロード
    instanceSBs_[currentFrameIndex_]->Update(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(GeneratedSchema::InstanceData)));
}

void Renderer::Render(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress) {
    if (queue_.empty()) return;

    auto& assetManager = Asset::AssetManager::GetInstance();
    auto& materialManager = Asset::MaterialManager::GetInstance();
    auto& textureManager = Asset::TextureManager::GetInstance();
    auto& shaderManager = ShaderManager::GetInstance();

    // 使用するインスタンスバッファを取得
    auto* currentSB = instanceSBs_[currentFrameIndex_].get();

    // 描画ループ（バッチング実行）
    uint32_t currentInstanceStart = 0;
    while (currentInstanceStart < queue_.size()) {
        const auto& batchStartReq = queue_[currentInstanceStart];
        
        // 同じModelとMaterialが続く範囲を特定
        uint32_t batchSize = 0;
        for (uint32_t i = currentInstanceStart; i < queue_.size(); ++i) {
            if (queue_[i].modelName == batchStartReq.modelName && 
                queue_[i].materialName == batchStartReq.materialName) {
                batchSize++;
            } else {
                break;
            }
        }

        // 描画設定
        auto* mat = materialManager.GetMaterial(batchStartReq.materialName);
        if (mat) {
            auto* pso = shaderManager.GetOrCreatePSO(mat->pipelineName, {});
            auto* rootSig = shaderManager.GetRootSignature(mat->pipelineName);
            
            commandList->SetGraphicsRootSignature(rootSig->Get());
            commandList->SetPipelineState(pso->Get());

            // 共通リソースのセット
            ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap()->GetHeap() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            commandList->SetGraphicsRootConstantBufferView(rootSig->GetParameterIndex("gSceneData"), sceneCBAddress);
            commandList->SetGraphicsRootDescriptorTable(rootSig->GetParameterIndex("gTextures"), textureManager.GetSrvHeap()->GetGPUHandle(0));
            
            // インスタンスバッファを現在のバッチの開始位置にオフセットしてセット
            D3D12_GPU_VIRTUAL_ADDRESS instBufferAddr = currentSB->GetResource()->GetGPUVirtualAddress();
            instBufferAddr += currentInstanceStart * sizeof(GeneratedSchema::InstanceData);
            commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gInstances"), instBufferAddr);

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // メッシュごとの描画
            const auto& meshes = assetManager.GetMeshes(batchStartReq.modelName);
            for (const auto& mesh : meshes) {
                commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gVertices"), mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
                
                D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBuffer()->GetView();
                commandList->IASetIndexBuffer(&ibv);
                
                // バッファ自体をオフセットしているため、StartInstance は 0 固定にする
                commandList->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), batchSize, 0, 0, 0);
            }
        }

        currentInstanceStart += batchSize;
    }
}

} // namespace Engine::Graphics
