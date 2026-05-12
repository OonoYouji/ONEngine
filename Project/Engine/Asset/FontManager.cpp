#include "FontManager.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/TextureManager.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"

namespace Engine::Asset {

FontManager* FontManager::instance_ = nullptr;

void FontManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;
}

void FontManager::Shutdown() {
    fontMap_.clear();
    indexedFonts_.clear();
}

int32_t FontManager::LoadFont(const std::string& pathOrGuid, float fontSize) {
    if (fontMap_.count(pathOrGuid)) {
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

    // グローバルヒープからハンドルを割り当て
    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics.GetSRVHeap();
    
    // インデックスを動的に割り当て
    uint32_t index = srvHeap->AllocateIndex();

    font->GetAtlasTexture()->CreateResource(device_, srvHeap->GetCPUHandle(index));
    font->GetAtlasTexture()->SetIndex(index);
    
    int32_t fontIndex = (int32_t)indexedFonts_.size();
    fontMap_[pathOrGuid] = font;
    indexedFonts_.push_back(font);

    return fontIndex;
}

Font* FontManager::GetFontByIndex(uint32_t index) {
    if (index >= indexedFonts_.size()) return nullptr;
    return indexedFonts_[index].get();
}

} // namespace Engine::Asset
