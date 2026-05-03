#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "AssetHandle.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;
class Texture;

///
/// Bindlessテクスチャを管理するクラス
///
class TextureManager {
public:
    static TextureManager& GetInstance() {
        static TextureManager instance;
        return instance;
    }

    void Initialize(RenderDevice* device);
    void Shutdown();

    /// @brief パスまたはGUIDからテクスチャをロード
    /// @param pathOrGuid ファイルパスまたはGUID
    /// @return ロードされたテクスチャのインデックス。失敗した場合は -1。
    int32_t LoadTexture(const std::string& pathOrGuid);

    /// @brief AssetRegistry経由でのロード
    std::shared_ptr<Texture> LoadTextureAsAsset(const std::string& pathOrGuid);

    /// @brief パスまたはGUIDからテクスチャを取得
    Texture* GetTexture(const std::string& pathOrGuid);

    DescriptorHeap* GetSrvHeap() const { return srvHeap_.get(); }

private:
    TextureManager();
    ~TextureManager();

    /// @brief パスOrGUIDをGUIDに変換
    std::string ToGuid(const std::string& pathOrGuid);

private:
    RenderDevice* device_ = nullptr;
    std::unique_ptr<DescriptorHeap> srvHeap_;
    
    // GUIDをキーにした管理（AssetRegistryと共有するか、こちらで実体を持ち続けるか）
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap_;
};

} // namespace Engine::Graphics
