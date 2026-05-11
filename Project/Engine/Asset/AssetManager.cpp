#include "AssetManager.h"
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
