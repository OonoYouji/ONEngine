#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Mesh.h"
#include "Model.h"
#include "AssetHandle.h"

namespace Engine::Graphics {
    class RenderDevice;
}

namespace Engine::Asset {

///
/// アセット全体を統合管理するクラス
///
class AssetManager {
public:
    static AssetManager& GetInstance() {
        static AssetManager instance;
        return instance;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief パスまたはGUIDからモデルをロードしてキャッシュ
    /// @param pathOrGuid ファイルパスまたはGUID
    /// @return モデルのインデックス
    int32_t LoadModel(const std::string& pathOrGuid);

    /// @brief AssetRegistry経由でのロード
    std::shared_ptr<Model> LoadModelAsAsset(const std::string& pathOrGuid);
    
    /// @brief インデックスからモデルを取得
    std::shared_ptr<Model> GetModelByIndex(uint32_t index);

    /// @brief ロード済みメッシュリストを取得
    const std::vector<std::unique_ptr<Mesh>>& GetMeshes(const std::string& pathOrGuid);
    const std::vector<std::unique_ptr<Mesh>>& GetMeshesByIndex(uint32_t index);

private:
    AssetManager() = default;
    ~AssetManager() = default;

    /// @brief 入力がパスかGUIDかを判定し、常にGUIDを返す
    std::string ToGuid(const std::string& pathOrGuid);

private:
    Graphics::RenderDevice* device_ = nullptr;
    // キャッシュはGUIDで管理
    std::unordered_map<std::string, std::shared_ptr<Model>> models_;
    std::vector<std::shared_ptr<Model>> indexedModels_;
};

} // namespace Engine::Asset
