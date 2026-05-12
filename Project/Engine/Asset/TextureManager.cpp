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
        return this->LoadTextureAsAsset(pathOrGuid);
    });

    Engine::Console::Log("TextureManager: Initialized using Global SRV Heap.");
}

void TextureManager::Shutdown() {
    textureMap_.clear();
}

int32_t TextureManager::LoadTexture(const std::string& filePath) {
    auto texture = LoadTextureAsAsset(filePath);
    if (!texture) return -1;
    return texture->GetIndex();
}

std::shared_ptr<Texture> TextureManager::LoadTextureAsAsset(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    if (textureMap_.count(guid)) return textureMap_[guid];

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid;

    auto texture = std::make_shared<Texture>();
    // GraphicsEngine のグローバルヒープを渡す
    auto* graphics = &Graphics::GraphicsEngine::GetInstance();
    auto* srvHeap = graphics->GetSRVHeap();
    
    // インデックスを動的に割り当て
    uint32_t index = srvHeap->AllocateIndex();

    if (texture->Load(Engine::ConvertString(path))) {
        texture->CreateResource(device_, srvHeap->GetCPUHandle(index));
        texture->SetIndex(index);
        textureMap_[guid] = texture;
        return texture;
    }

    return nullptr;
}

Texture* TextureManager::GetTexture(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    if (textureMap_.count(guid)) return textureMap_[guid].get();
    return nullptr;
}

std::string TextureManager::ToGuid(const std::string& pathOrGuid) {
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) != "") return pathOrGuid;
    std::string guid = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
    return (guid != "") ? guid : pathOrGuid;
}

} // namespace Engine::Asset
