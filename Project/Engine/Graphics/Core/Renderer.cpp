#include "Renderer.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Resource/AssetManager.h"
#include "Engine/Graphics/Resource/MaterialManager.h"
#include "Engine/Graphics/Resource/TextureManager.h"
#include "Engine/Graphics/Shader/ShaderManager.h"
#include "Engine/Graphics/Resource/Mesh.h"
#include "Engine/Graphics/Resource/Texture.h"
#include "Engine/Common/Console.h"
#include <algorithm>
#include "Engine/Graphics/Core/DescriptorHeap.h"

namespace Engine::Graphics {

void Renderer::Initialize(RenderDevice* device) {
    device_ = device;
    instanceSB_ = std::make_unique<StructuredBuffer>();
    instanceSB_->Create(device, sizeof(GeneratedSchema::InstanceData), kMaxInstances);
}

void Renderer::Shutdown() {
    instanceSB_.reset();
}

void Renderer::PushRequest(const RenderRequest& request) {
    if (queue_.size() < kMaxInstances) {
        queue_.push_back(request);
    }
}

void Renderer::ClearQueue() {
    queue_.clear();
}

void Renderer::Render(ID3D12GraphicsCommandList* commandList, const D3D12_GPU_VIRTUAL_ADDRESS sceneCBAddress) {
    if (queue_.empty()) return;

    auto& assetManager = AssetManager::GetInstance();
    auto& materialManager = MaterialManager::GetInstance();
    auto& textureManager = TextureManager::GetInstance();
    auto& shaderManager = ShaderManager::GetInstance();

    // 1. ソートしてバッチングしやすくする（Model -> Material順）
    std::sort(queue_.begin(), queue_.end(), [](const RenderRequest& a, const RenderRequest& b) {
        if (a.modelName != b.modelName) return a.modelName < b.modelName;
        return a.materialName < b.materialName;
    });

    // 2. 全インスタンスデータの抽出とアップロード
    std::vector<GeneratedSchema::InstanceData> instanceData;
    instanceData.reserve(queue_.size());
    for (const auto& req : queue_) {
        GeneratedSchema::InstanceData data;
        data.world = req.world;
        
        auto* mat = materialManager.GetMaterial(req.materialName);
        if (mat) {
            auto* tex = textureManager.GetTexture(mat->textureName);
            if (tex) {
                data.textureIndex = tex->GetIndex();
            } else {
                Engine::Console::LogWarning(std::format("Renderer: Texture '{}' not found for material '{}'", mat->textureName, req.materialName));
                data.textureIndex = 0; // ここでエラー回避用のデフォルトテクスチャを指定するのが理想
            }
        } else {
            data.textureIndex = 0;
        }
        instanceData.push_back(data);
    }
    instanceSB_->Update(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(GeneratedSchema::InstanceData)));

    // 3. 描画ループ（バッチング実行）
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
            
            // 全体のインスタンスバッファをセット（シェーダー側で SV_InstanceID でオフセットを考慮してアクセスするか、
            // もしくはバッファ自体をオフセットして渡す。今回は単純化のため全体を渡し、Drawの引数で制御）
            commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gInstances"), instanceSB_->GetResource()->GetGPUVirtualAddress());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // メッシュごとの描画
            const auto& meshes = assetManager.GetMeshes(batchStartReq.modelName);
            for (const auto& mesh : meshes) {
                commandList->SetGraphicsRootShaderResourceView(rootSig->GetParameterIndex("gVertices"), mesh->GetVertexBuffer()->GetResource()->GetGPUVirtualAddress());
                
                D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBuffer()->GetView();
                commandList->IASetIndexBuffer(&ibv);
                
                // DrawIndexedInstanced(IndexCount, InstanceCount, StartIndex, BaseVertex, StartInstance)
                // StartInstance を指定することで、StructuredBuffer内の正しい位置からデータが読み込まれる
                commandList->DrawIndexedInstanced(mesh->GetIndexBuffer()->GetCount(), batchSize, 0, 0, currentInstanceStart);
            }
        }

        currentInstanceStart += batchSize;
    }
}

} // namespace Engine::Graphics
