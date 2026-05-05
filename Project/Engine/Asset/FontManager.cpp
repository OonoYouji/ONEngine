#include "FontManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"

namespace Engine::Asset {

void FontManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;
}

void FontManager::Shutdown() {
    fontMap_.clear();
    indexedFonts_.clear();
}

int32_t FontManager::LoadFont(const std::string& pathOrGuid, float fontSize) {
    if (fontMap_.count(pathOrGuid)) {
        // すでにロード済みならインデックスを返す
        auto font = fontMap_[pathOrGuid];
        for (int32_t i = 0; i < (int32_t)indexedFonts_.size(); ++i) {
            if (indexedFonts_[i] == font) return i;
        }
    }

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid);
    if (path == "") path = pathOrGuid;

    auto font = std::make_shared<Font>();
    if (!font->Load(Engine::ConvertString(path), fontSize)) {
        return -1;
    }

    // フォントアトラスのテクスチャにBindlessインデックスを割り当てる
    // TextureManagerを介してSRVを生成
    auto& textureManager = TextureManager::GetInstance();
    uint32_t bindlessIndex = textureManager.GetSrvHeap()->AllocateIndex();
    font->GetAtlasTexture()->SetIndex(bindlessIndex);
    font->GetAtlasTexture()->CreateResource(device_, textureManager.GetSrvHeap()->GetCPUHandle(bindlessIndex));

    int32_t index = (int32_t)indexedFonts_.size();
    fontMap_[pathOrGuid] = font;
    indexedFonts_.push_back(font);

    return index;
}

Font* FontManager::GetFontByIndex(uint32_t index) {
    if (index >= indexedFonts_.size()) return nullptr;
    return indexedFonts_[index].get();
}

} // namespace Engine::Asset
