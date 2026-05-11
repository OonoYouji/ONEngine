#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Font.h"

namespace Engine::Graphics {
    class RenderDevice;
}

namespace Engine::Asset {

///
/// フォントアセットを管理するクラス
///
class FontManager {
public:
    static FontManager& GetInstance() {
        return *instance_;
    }

    static void CreateInstance() {
        if (!instance_) instance_ = new FontManager();
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief フォントをロードし、インデックスを返す
    int32_t LoadFont(const std::string& pathOrGuid, float fontSize = 32.0f);

    /// @brief インデックスからフォントを取得
    Font* GetFontByIndex(uint32_t index);

private:
    FontManager() = default;
    ~FontManager() = default;

    static FontManager* instance_;

    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Font>> fontMap_;
    std::vector<std::shared_ptr<Font>> indexedFonts_;
};

} // namespace Engine::Asset
