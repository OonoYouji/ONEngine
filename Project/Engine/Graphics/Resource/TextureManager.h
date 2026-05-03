#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Texture.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

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

    /// @brief ファイルからテクスチャをロードして登録
    /// @param name 登録名
    /// @param filePath ファイルパス
    /// @return 成功時にテクスチャのBindless Indexを返す。失敗時は -1。
    int32_t LoadTexture(const std::string& name, const std::wstring& filePath);

    /// @brief 登録済みのテクスチャを取得
    Texture* GetTexture(const std::string& name);

    /// @brief 全テクスチャが登録されたSRVヒープを取得
    DescriptorHeap* GetSrvHeap() const { return srvHeap_.get(); }

private:
    TextureManager();
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    RenderDevice* device_ = nullptr;
    std::unique_ptr<DescriptorHeap> srvHeap_;
    std::vector<std::unique_ptr<Texture>> texturePool_;
    std::unordered_map<std::string, Texture*> textureMap_;
};

} // namespace Engine::Graphics
