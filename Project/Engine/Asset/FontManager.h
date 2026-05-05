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
        static FontManager instance;
        return instance;
    }

    void Initialize(Graphics::RenderDevice* device);
    void Shutdown();

    /// @brief TTFファイルからフォントをロード
    int32_t LoadFont(const std::string& pathOrGuid, float fontSize = 32.0f);

    /// @brief インデックスからフォントを取得
    Font* GetFontByIndex(uint32_t index);

private:
    FontManager() = default;
    ~FontManager() = default;

private:
    Graphics::RenderDevice* device_ = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Font>> fontMap_;
    std::vector<std::shared_ptr<Font>> indexedFonts_;
};

} // namespace Engine::Asset
