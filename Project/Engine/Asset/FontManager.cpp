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
    uint64_t guid = 0;
    if (!pathOrGuid.empty() && std::all_of(pathOrGuid.begin(), pathOrGuid.end(), ::isdigit)) {
        guid = std::stoull(pathOrGuid);
    } else {
        guid = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
    }

    if (guid != 0) return LoadFont(guid, fontSize);

    // GUIDがない場合
    return -1;
}

int32_t FontManager::LoadFont(uint64_t guid, float fontSize) {
    if (guid == 0) return -1;
    if (fontMap_.count(guid)) {
        auto font = fontMap_[guid];
        for (int32_t i = 0; i < (int32_t)indexedFonts_.size(); ++i) {
            if (indexedFonts_[i] == font) return i;
        }
    }

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") return -1;

    auto font = std::make_shared<Font>();
    if (!font->Load(Engine::ConvertString(path), fontSize)) {
        return -1;
    }

    auto& graphics = Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics.GetSRVHeap();
    uint32_t index = srvHeap->AllocateIndex();

    font->GetAtlasTexture()->CreateResource(device_, srvHeap->GetCPUHandle(index));
    font->GetAtlasTexture()->SetIndex(index);
    font->SetGuid(guid);
    font->SetPath(path);
    
    int32_t fontIndex = (int32_t)indexedFonts_.size();
    fontMap_[guid] = font;
    indexedFonts_.push_back(font);

    return fontIndex;
}

Font* FontManager::GetFontByIndex(uint32_t index) {
    if (index >= indexedFonts_.size()) return nullptr;
    return indexedFonts_[index].get();
}

Font* FontManager::GetFontByGuid(uint64_t guid) {
    auto it = fontMap_.find(guid);
    return (it != fontMap_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Asset
