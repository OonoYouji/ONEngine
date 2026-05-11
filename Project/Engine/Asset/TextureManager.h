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

    /// @brief インデックスからテクスチャを取得
    Texture* GetTexture(const std::string& pathOrGuid);

    std::shared_ptr<Texture> LoadTextureAsAsset(const std::string& pathOrGuid);

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager* instance_;

    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap_;

    std::string ToGuid(const std::string& pathOrGuid);
};

} // namespace Engine::Asset
