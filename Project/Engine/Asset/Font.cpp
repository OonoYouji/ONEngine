#include "Font.h"
#include <fstream>
#include <vector>
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Common/Console.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "Externals/imgui/imstb_truetype.h"

namespace Engine::Asset {

Font::Font() = default;
Font::~Font() = default;

bool Font::Load(const std::wstring& filePath, float fontSize) {
    path_ = Engine::ConvertString(filePath);
    state_ = AssetLoadState::Loading;

    // 1. TTFファイルをバイナリとして読み込む
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;
    
    size_t size = file.tellg();
    std::vector<unsigned char> ttfBuffer(size);
    file.seekg(0);
    file.read((char*)ttfBuffer.data(), size);
    file.close();

    // 2. stb_truetype を使ってアトラスを生成
    std::vector<unsigned char> bitmap(kAtlasWidth * kAtlasHeight);
    stbtt_bakedchar bakedChars[96]; // ASCII 32-126
    
    int result = stbtt_BakeFontBitmap(ttfBuffer.data(), 0, fontSize, bitmap.data(), kAtlasWidth, kAtlasHeight, 32, 96, bakedChars);
    if (result <= 0) {
        Engine::Console::LogError("Font: Failed to bake font bitmap.");
        state_ = AssetLoadState::Error;
        return false;
    }

    // 3. テクスチャの作成
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    atlasTexture_ = std::make_unique<Texture>();
    if (!atlasTexture_->CreateFromPixels(graphics.GetRenderDevice(), bitmap.data(), kAtlasWidth, kAtlasHeight, DXGI_FORMAT_R8_UNORM)) {
        state_ = AssetLoadState::Error;
        return false;
    }

    // 4. 文字情報の保存
    for (int i = 0; i < 96; ++i) {
        char c = (char)(32 + i);
        GlyphInfo info;
        info.uvMinX = (float)bakedChars[i].x0 / kAtlasWidth;
        info.uvMinY = (float)bakedChars[i].y0 / kAtlasHeight;
        info.uvMaxX = (float)bakedChars[i].x1 / kAtlasWidth;
        info.uvMaxY = (float)bakedChars[i].y1 / kAtlasHeight;
        
        info.width = (float)(bakedChars[i].x1 - bakedChars[i].x0);
        info.height = (float)(bakedChars[i].y1 - bakedChars[i].y0);
        info.xOffset = bakedChars[i].xoff;
        info.yOffset = bakedChars[i].yoff;
        info.xAdvance = bakedChars[i].xadvance;
        
        glyphs_[c] = info;
    }

    state_ = AssetLoadState::Ready;
    Engine::Console::Log(std::format("Font: Loaded and rasterized {}", path_));
    return true;
}

const GlyphInfo* Font::GetGlyph(char c) const {
    auto it = glyphs_.find(c);
    return (it != glyphs_.end()) ? &it->second : nullptr;
}

} // namespace Engine::Asset
