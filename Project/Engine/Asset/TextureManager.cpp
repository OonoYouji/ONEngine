#include "TextureManager.h"
#include "AssetDatabase.h"
#include "Texture.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/CommandQueue.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Asset/AssetRegistry.h"
#include "Externals/nlohmann/json.hpp"

namespace Engine::Asset {

TextureManager* TextureManager::instance_ = nullptr;

const uint32_t kMaxBindlessTextures = 1024;

void TextureManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;

    // AssetRegistry への登録
    AssetRegistry::GetInstance().RegisterType<Texture>(AssetType::Texture);
    AssetRegistry::GetInstance().RegisterLoader(AssetType::Texture, [this](const std::string& pathOrGuid) {
        if (!pathOrGuid.empty() && std::all_of(pathOrGuid.begin(), pathOrGuid.end(), ::isdigit)) {
            return this->LoadTextureAsAsset(std::stoull(pathOrGuid));
        }
        return this->LoadTextureAsAsset(pathOrGuid);
    });

    Engine::Console::Log("TextureManager: Initialized using Global SRV Heap.");
}

void TextureManager::Shutdown() {
    textureMap_.clear();
    indexedTextures_.clear();
}

int32_t TextureManager::LoadTexture(const std::string& filePath) {
    auto texture = LoadTextureAsAsset(filePath);
    if (!texture) return -1;
    return texture->GetIndex();
}

int32_t TextureManager::LoadTexture(uint64_t guid) {
    auto texture = LoadTextureAsAsset(guid);
    if (!texture) return -1;
    return texture->GetIndex();
}

std::shared_ptr<Texture> TextureManager::LoadTextureAsAsset(uint64_t guid) {
    if (guid == 0) return nullptr;
    if (textureMap_.count(guid)) return textureMap_[guid];

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") return nullptr;

    auto texture = std::make_shared<Texture>();
    auto* graphics = &Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics->GetSRVHeap();
    
    uint32_t index = srvHeap->AllocateIndex();

    if (texture->Load(Engine::ConvertString(path))) {
        texture->CreateResource(device_, srvHeap->GetCPUHandle(index));
        texture->SetIndex(index);
        texture->SetGuid(guid);
        texture->SetPath(path);
        textureMap_[guid] = texture;
        
        // indexedTextures_ にも追加
        if (index >= indexedTextures_.size()) indexedTextures_.resize(index + 1);
        indexedTextures_[index] = texture;
        
        return texture;
    }

    return nullptr;
}

std::shared_ptr<Texture> TextureManager::LoadTextureAsAsset(const std::string& path) {
    uint64_t guid = AssetDatabase::GetInstance().GetGuidFromPath(path);
    if (guid != 0) return LoadTextureAsAsset(guid);

    // GUIDがない場合（未登録アセット）
    auto texture = std::make_shared<Texture>();
    auto* graphics = &Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics->GetSRVHeap();
    uint32_t index = srvHeap->AllocateIndex();

    if (texture->Load(Engine::ConvertString(path))) {
        texture->CreateResource(device_, srvHeap->GetCPUHandle(index));
        texture->SetIndex(index);
        texture->SetPath(path);
        
        if (index >= indexedTextures_.size()) indexedTextures_.resize(index + 1);
        indexedTextures_[index] = texture;
        
        return texture;
    }
    return nullptr;
}

Texture* TextureManager::GetTexture(uint64_t guid) {
    if (textureMap_.count(guid)) return textureMap_[guid].get();
    return nullptr;
}

Texture* TextureManager::GetTextureByIndex(uint32_t index) {
    if (index >= indexedTextures_.size()) return nullptr;
    return indexedTextures_[index].get();
}

} // namespace Engine::Asset
