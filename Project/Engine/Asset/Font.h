#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "IAsset.h"
#include "Texture.h"
#include "Externals/imgui/imstb_truetype.h"

namespace Engine::Asset {

///
/// フォント文字の情報を保持する構造体
///
struct GlyphInfo {
    float uvMinX, uvMinY;
    float uvMaxX, uvMaxY;
    float width, height;
    float xOffset, yOffset;
    float xAdvance;
};

///
/// フォントアセット（TTFをラスタライズして保持）
///
class Font : public IAsset {
public:
    Font();
    virtual ~Font();

    AssetType GetType() const override { return AssetType::Font; }

    /// @brief TTFファイルをロードしてアトラスを生成
    bool Load(const std::wstring& filePath, float fontSize = 32.0f);

    /// @brief 文字情報を取得
    const GlyphInfo* GetGlyph(char c) const;

    /// @brief 生成されたアトラスのテクスチャを取得
    Texture* GetAtlasTexture() const { return atlasTexture_.get(); }

private:
    std::unique_ptr<Texture> atlasTexture_;
    std::map<char, GlyphInfo> glyphs_;
    
    static constexpr int kAtlasWidth = 512;
    static constexpr int kAtlasHeight = 512;
};

} // namespace Engine::Asset
