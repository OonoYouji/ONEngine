#include "TextureManager.h"
#include "AssetDatabase.h"
#include "Texture.h"
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Common/Console.h"
#include "Engine/Graphics/Core/CommandQueue.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "AssetRegistry.h"
#include "Externals/nlohmann/json.hpp"
#include <d3dx12.h>
#include <DirectXTex.h>
#include <fstream>

using json = nlohmann::json;

namespace Engine::Asset {

const uint32_t kMaxBindlessTextures = 1024;

TextureManager::TextureManager() = default;
TextureManager::~TextureManager() = default;

void TextureManager::Initialize(Graphics::RenderDevice* device) {
    device_ = device;
    srvHeap_ = std::make_unique<Graphics::DescriptorHeap>();
    srvHeap_->Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxBindlessTextures, true);

    // AssetRegistryへの登録
    AssetRegistry::GetInstance().RegisterType<Texture>(AssetType::Texture);
    AssetRegistry::GetInstance().RegisterLoader(AssetType::Texture, [this](const std::string& pathOrGuid) {
        return this->LoadTextureAsAsset(pathOrGuid);
    });
}

void TextureManager::Shutdown() {
    textureMap_.clear();
    srvHeap_.reset();
}

std::string TextureManager::ToGuid(const std::string& pathOrGuid) {
    if (AssetDatabase::GetInstance().GetPathFromGuid(pathOrGuid) != "") return pathOrGuid;
    std::string guid = AssetDatabase::GetInstance().GetGuidFromPath(pathOrGuid);
    return (guid != "") ? guid : pathOrGuid;
}

int32_t TextureManager::LoadTexture(const std::string& pathOrGuid) {
    auto texture = LoadTextureAsAsset(pathOrGuid);
    return texture ? texture->GetIndex() : -1;
}

std::shared_ptr<Texture> TextureManager::LoadTextureAsAsset(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    
    if (textureMap_.count(guid)) {
        return textureMap_[guid];
    }

    if (textureMap_.size() >= kMaxBindlessTextures) {
        Engine::Console::LogError("Texture pool is full.");
        return nullptr;
    }

    std::string path = AssetDatabase::GetInstance().GetPathFromGuid(guid);
    if (path == "") path = pathOrGuid;

    // --- インポート設定の読み込み ---
    bool isSRGB = true; // デフォルト
    std::string metaPath = path + ".meta";
    std::ifstream metaFile(metaPath);
    if (metaFile.is_open()) {
        try {
            json metaData = json::parse(metaFile);
            if (metaData.contains("colorSpace")) {
                std::string colorSpace = metaData["colorSpace"];
                isSRGB = (colorSpace == "sRGB");
            }
        } catch (...) {}
    }

    auto texture = std::make_shared<Texture>();
    std::wstring wpath = Engine::ConvertString(path);
    
    if (!texture->Load(wpath)) {
        return nullptr;
    }

    uint32_t index = static_cast<uint32_t>(textureMap_.size());
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap_->GetCPUHandle(index);
    texture->CreateResource(device_, srvHandle);
    texture->SetIndex(index);

    // データ転送
    auto* image = texture->GetImage();
    auto* res = texture->GetResource();
    const auto& metadata = image->GetMetadata();
    
    // もし Meta で強制的なフォーマット指定があればここで上書き等の処理を行う
    
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    if (SUCCEEDED(DirectX::PrepareUpload(device_->GetDevice(), image->GetImages(), image->GetImageCount(), metadata, subresources))) {
        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(res, 0, static_cast<UINT>(subresources.size()));
        ComPtr<ID3D12Resource> uploadHeap;
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        device_->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadHeap));

        auto* queue = Graphics::GraphicsEngine::GetInstance().GetCommandQueue();
        queue->Reset();
        auto* commandList = queue->GetCommandList();
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &barrier);
        UpdateSubresources(commandList, res, uploadHeap.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);
        queue->Execute();
        queue->SignalAndWait();
    }

    texture->SetIndex(index);
    textureMap_[guid] = texture;

    Engine::Console::Log(std::format("TextureManager: Loaded [{}] (sRGB: {})", path, isSRGB));
    return texture;
}

Texture* TextureManager::GetTexture(const std::string& pathOrGuid) {
    std::string guid = ToGuid(pathOrGuid);
    auto it = textureMap_.find(guid);
    return (it != textureMap_.end()) ? it->second.get() : nullptr;
}

} // namespace Engine::Asset
