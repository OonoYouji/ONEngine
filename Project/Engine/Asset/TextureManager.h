#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <d3d12.h>
#include "Asset/Texture.h"

namespace Engine::Graphics {
    class RenderDevice;
}

namespace Engine::Asset {

///
/// テクスチャアセットを一括管理するクラス
///
class TextureManager {
public:
    static TextureManager& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new TextureManager();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief テクスチャをロードし、インデックスを返す
    int32_t LoadTexture(const std::string& filePath);
    int32_t LoadTexture(uint64_t guid);

    /// @brief インデックスからテクスチャを取得
    Texture* GetTexture(uint64_t guid);
    Texture* GetTextureByIndex(uint32_t index);

    std::shared_ptr<Texture> LoadTextureAsAsset(uint64_t guid);
    std::shared_ptr<Texture> LoadTextureAsAsset(const std::string& path);

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager* instance_;

    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<uint64_t, std::shared_ptr<Texture>> textureMap_;
    std::vector<std::shared_ptr<Texture>> indexedTextures_;
};

} // namespace Engine::Asset
