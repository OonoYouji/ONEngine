#pragma once

#include <string>

namespace Engine::Asset {

///
/// アセットの種類を定義する列挙型
///
enum class AssetType {
    Unknown,
    Texture,
    Material,
    Mesh,
    Model,
    Shader,
    Audio,
    Font,
    Scene,
    Prefab
};

///
/// ロード状態を表す列挙型
///
enum class AssetLoadState {
    Unloaded,
    Loading,
    Ready,
    Error
};

///
/// すべてのアセットの基底インターフェース
///
class IAsset {
public:
    virtual ~IAsset() = default;

    /// @brief アセットの種類を取得
    virtual AssetType GetType() const = 0;

    /// @brief GUIDを取得
    const std::string& GetGuid() const { return guid_; }
    void SetGuid(const std::string& guid) { guid_ = guid; }

    /// @brief ファイルパスを取得
    const std::string& GetPath() const { return path_; }
    void SetPath(const std::string& path) { path_ = path; }

    /// @brief ロード状態を取得
    AssetLoadState GetLoadState() const { return state_; }

protected:
    std::string guid_;
    std::string path_;
    AssetLoadState state_ = AssetLoadState::Unloaded;
};

} // namespace Engine::Asset
