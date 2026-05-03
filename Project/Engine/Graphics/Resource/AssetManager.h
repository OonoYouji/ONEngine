#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Engine/Graphics/Resource/Mesh.h"

namespace Engine::Graphics {
    //class Mesh;
    class RenderDevice;
}

namespace Engine {

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
    void LoadModel(const std::string& pathOrGuid);
    
    /// @brief ロード済みメッシュリストを取得
    const std::vector<std::unique_ptr<Graphics::Mesh>>& GetMeshes(const std::string& pathOrGuid);

private:
    AssetManager() = default;
    ~AssetManager() = default;

    /// @brief 入力がパスかGUIDかを判定し、常にGUIDを返す
    std::string ToGuid(const std::string& pathOrGuid);

private:
    Graphics::RenderDevice* device_ = nullptr;
    // キャッシュはGUIDで管理
    std::unordered_map<std::string, std::vector<std::unique_ptr<Graphics::Mesh>>> models_;
};

} // namespace Engine
