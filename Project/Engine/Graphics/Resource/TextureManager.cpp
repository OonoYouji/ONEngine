#include "TextureManager.h"
#include <DirectXTex.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/DescriptorHeap.h"
#include "Engine/Common/Console.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/CommandQueue.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"

namespace Engine::Graphics {

// Bindlessで確保するテクスチャの最大数
const uint32_t kMaxBindlessTextures = 1024;

TextureManager::TextureManager() = default;
TextureManager::~TextureManager() = default;

void TextureManager::Initialize(RenderDevice* device) {
    device_ = device;
    srvHeap_ = std::make_unique<DescriptorHeap>();
    srvHeap_->Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxBindlessTextures, true);
    texturePool_.reserve(kMaxBindlessTextures);
}

void TextureManager::Shutdown() {
    textureMap_.clear();
    texturePool_.clear();
    srvHeap_.reset();
}

int32_t TextureManager::LoadTexture(const std::string& name, const std::wstring& filePath) {
    if (textureMap_.count(name)) {
        return textureMap_[name]->GetIndex();
    }

    if (texturePool_.size() >= kMaxBindlessTextures) {
        Engine::Console::LogError("Texture pool is full. Cannot load more textures.");
        return -1;
    }

    auto texture = std::make_unique<Texture>();
    if (!texture->Load(filePath)) {
        return -1;
    }

    // SRVヒープの次の空きスロットにSRVを作成
    uint32_t index = static_cast<uint32_t>(texturePool_.size());
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap_->GetCPUHandle(index);
    texture->CreateResource(device_, srvHandle);

    // GPUへのデータアップロード
    auto* image = texture->GetImage();
    auto* res = texture->GetResource();
    const auto& metadata = image->GetMetadata();
    
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    HRESULT hr = DirectX::PrepareUpload(device_->GetDevice(), image->GetImages(), image->GetImageCount(), metadata, subresources);
    if (SUCCEEDED(hr)) {
        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(res, 0, static_cast<UINT>(subresources.size()));
        
        ComPtr<ID3D12Resource> uploadHeap;
        auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        device_->GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadHeap));

        auto* queue = GraphicsEngine::GetInstance().GetCommandQueue();
        queue->Reset();
        auto* commandList = queue->GetCommandList();

        // 状態を明示的に強制 (COMMON -> COPY_DEST)
        // 初期状態が不明な場合もあるため、一度バリアを張る
        D3D12_RESOURCE_BARRIER preBarrier = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &preBarrier);

        UpdateSubresources(commandList, res, uploadHeap.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
        
        // COPY_DEST -> PIXEL_SHADER_RESOURCE
        D3D12_RESOURCE_BARRIER postBarrier = CD3DX12_RESOURCE_BARRIER::Transition(res, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &postBarrier);

        // アップロード完了まで待機（uploadHeapがスコープを抜ける前に必ず待つ）
        queue->Execute();
        queue->SignalAndWait();
        
        Engine::Console::Log(std::format("Uploaded Texture: {}x{}, Mips: {}, Format: {}", metadata.width, metadata.height, metadata.mipLevels, (int)metadata.format));
    } else {
        Engine::Console::LogError("Failed to prepare texture upload.");
    }

    texture->SetIndex(index);
    textureMap_[name] = texture.get();
    texturePool_.push_back(std::move(texture));

    Engine::Console::Log(std::format("Texture '{}' loaded and uploaded. Assigned Index: {}", name, index));
    return index;
}

Texture* TextureManager::GetTexture(const std::string& name) {
    auto it = textureMap_.find(name);
    return (it != textureMap_.end()) ? it->second : nullptr;
}

} // namespace Engine::Graphics
