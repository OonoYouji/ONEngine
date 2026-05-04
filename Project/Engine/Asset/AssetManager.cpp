#include "AssetManager.h"
#include "AssetDatabase.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "AssetRegistry.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"

namespace Engine::Asset {

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
    // もし既に登録済みのGUIDならそのまま返す
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) != "") {
        return pathOrGuid;
    }
    // パスとして扱ってGUIDを取得
    std::string guid = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
    if (guid != "") return guid;

    // 見つからない場合は入力自体を返す（未登録アセットの場合のフォールバック）
    return pathOrGuid;
}

int32_t AssetManager::LoadModel(const std::string& pathOrGuid) {
    auto model = LoadModelAsAsset(pathOrGuid);
    if (!model) return -1;

    for (size_t i = 0; i < indexedModels_.size(); ++i) {
        if (indexedModels_[i]->GetGuid() == model->GetGuid()) return static_cast<int32_t>(i);
    }

    indexedModels_.push_back(model);
    return static_cast<int32_t>(indexedModels_.size() - 1);
}

std::shared_ptr<Model> AssetManager::LoadModelAsAsset(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    if (models_.count(guid)) return models_[guid];
    
    // ロードには実際のパスが必要
    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid; // GUIDで見つからなければパスとして扱う

    auto meshes = ModelLoader::LoadModel(device_, path);
    if (!meshes.empty()) {
        auto model = std::make_shared<Model>();
        model->SetGuid(guid);
        model->SetPath(path);
        model->SetMeshes(std::move(meshes));
        models_[guid] = model;
        Engine::Console::Log(std::format("AssetManager: Loaded Model [{}] from {}", guid, path));
        return model;
    }
    return nullptr;
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
