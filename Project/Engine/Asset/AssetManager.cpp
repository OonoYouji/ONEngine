#include "AssetManager.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "AssetDatabase.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"

namespace Engine::Asset {

AssetManager* AssetManager::instance_ = nullptr;

void AssetManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    meshInfoBuffer_ = std::make_unique<Graphics::StructuredBuffer>();
    meshInfoBuffer_->Create(device, sizeof(GeneratedSchema::MeshInfo), 1024, nullptr, false);

    // 起動時にAssetsディレクトリをスキャン
    AssetDatabase::GetInstance().Scan("Project/Assets");
    AssetDatabase::GetInstance().Scan("Project/Packages");

    // AssetRegistryへの登録
    AssetRegistry::GetInstance().RegisterType<Model>(AssetType::Model);
    AssetRegistry::GetInstance().RegisterLoader(AssetType::Model, [this](const std::string& pathOrGuid) {
        return this->LoadModelAsAsset(pathOrGuid);
    });
}

void AssetManager::Shutdown() {
    models_.clear();
    indexedModels_.clear();
    meshInfoBuffer_.reset();
}

D3D12_GPU_VIRTUAL_ADDRESS AssetManager::GetMeshInfoBufferAddress() const {
    return meshInfoBuffer_->GetResource()->GetGPUVirtualAddress();
}

void AssetManager::UpdateMeshInfoBuffer() {
    std::vector<GeneratedSchema::MeshInfo> infos(1024);
    for (uint32_t i = 0; i < (uint32_t)indexedModels_.size() && i < 1024; ++i) {
        const auto& meshes = indexedModels_[i]->GetMeshes();
        if (meshes.empty()) continue;

        // モデル全体の代表情報を 0-255 (i) に格納
        infos[i].vertexCount = 0;
        infos[i].indexCount = 0;
        infos[i].meshCount = static_cast<uint32_t>(meshes.size());
        infos[i].vertexOffset = 256 + i * 16; // サブメッシュ情報へのオフセット (簡易版)

        // 各サブメッシュの情報を 256+ に格納
        for (uint32_t m = 0; m < (uint32_t)meshes.size() && m < 16; ++m) {
            uint32_t subIdx = 256 + i * 16 + m;
            if (subIdx >= 1024) break;
            infos[subIdx].vertexOffset = meshes[m]->GetVertexOffset();
            infos[subIdx].indexOffset = meshes[m]->GetIndexOffset();
            infos[subIdx].vertexCount = meshes[m]->GetVertexCount();
            infos[subIdx].indexCount = meshes[m]->GetIndexCount();
        }
    }
    meshInfoBuffer_->Update(infos.data(), (uint32_t)(infos.size() * sizeof(GeneratedSchema::MeshInfo)));
}

std::string AssetManager::ToGuid(const std::string& pathOrGuid) {
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) != "") return pathOrGuid;
    std::string guid = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
    return (guid != "") ? guid : pathOrGuid;
}

uint32_t AssetManager::LoadModel(const std::string& pathOrGuid) {
    auto model = LoadModelAsAsset(pathOrGuid);
    if (!model) return 0xFFFFFFFF;
    
    // インデックス管理
    for (uint32_t i = 0; i < (uint32_t)indexedModels_.size(); ++i) {
        if (indexedModels_[i] == model) return i;
    }
    
    indexedModels_.push_back(model);
    return (uint32_t)indexedModels_.size() - 1;
}

std::shared_ptr<Model> AssetManager::LoadModelAsAsset(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    if (models_.count(guid)) return models_[guid];
    
    // ロードには実際のパスが必要
    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid; // GUIDで見つからなければパスとして扱う

    auto model = ModelLoader::LoadModel(device_, path);
    if (model) {
        model->SetGuid(guid);
        model->SetPath(path);
        models_[guid] = model;
        return model;
    }
    return nullptr;
}

std::shared_ptr<Model> AssetManager::GetModelByIndex(uint32_t index) {
    if (index >= indexedModels_.size()) return nullptr;
    return indexedModels_[index];
}

const std::vector<std::unique_ptr<Mesh>>& AssetManager::GetMeshes(const std::string& pathOrGuid) {
    static std::vector<std::unique_ptr<Mesh>> empty;
    std::string guid = ToGuid(pathOrGuid);
    auto it = models_.find(guid);
    return (it != models_.end()) ? it->second->GetMeshes() : empty;
}

const std::vector<std::unique_ptr<Mesh>>& AssetManager::GetMeshesByIndex(uint32_t index) {
    static std::vector<std::unique_ptr<Mesh>> empty;
    if (index >= indexedModels_.size()) return empty;
    return indexedModels_[index]->GetMeshes();
}

} // namespace Engine::Asset
