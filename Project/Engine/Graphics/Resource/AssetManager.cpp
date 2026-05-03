#include "AssetManager.h"
#include "AssetDatabase.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Console.h"

namespace Engine {

void AssetManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;
    // 起動時にAssetsディレクトリをスキャン
    AssetDatabase::GetInstance().Scan("Project/Assets");
    AssetDatabase::GetInstance().Scan("Project/Packages");
}

void AssetManager::Shutdown() {
    models_.clear();
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

void AssetManager::LoadModel(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    if (models_.count(guid)) return;
    
    // ロードには実際のパスが必要
    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid; // GUIDで見つからなければパスとして扱う

    auto meshes = Graphics::ModelLoader::LoadModel(device_, path);
    if (!meshes.empty()) {
        models_[guid] = std::move(meshes);
        Engine::Console::Log(std::format("AssetManager: Loaded Model [{}] from {}", guid, path));
    }
}

const std::vector<std::unique_ptr<Graphics::Mesh>>& AssetManager::GetMeshes(const std::string& pathOrGuid) {
    static std::vector<std::unique_ptr<Graphics::Mesh>> empty;
    std::string guid = ToGuid(pathOrGuid);
    auto it = models_.find(guid);
    return (it != models_.end()) ? it->second : empty;
}

} // namespace Engine
