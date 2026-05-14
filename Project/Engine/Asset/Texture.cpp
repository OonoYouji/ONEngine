#include "Texture.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include <filesystem>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Graphics/Core/GraphicsEngine.h"
#include "Engine/Graphics/Core/CommandQueue.h"
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

namespace Engine::Asset {

Texture::Texture() = default;
Texture::~Texture() = default;

bool Texture::Load(const std::wstring& filePath) {
    path_ = Engine::ConvertString(filePath);
    state_ = AssetLoadState::Loading;
    
    image_ = std::make_unique<DirectX::ScratchImage>();
    std::wstring ext = std::filesystem::path(filePath).extension();

    HRESULT hr;
    if (_wcsicmp(ext.c_str(), L".dds") == 0) {
        hr = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, *image_);
    } else {
        hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *image_);
    }

    if (FAILED(hr)) {
        Engine::Console::LogError(Engine::ConvertString(std::format(L"Failed to load texture file: {}", filePath)));
        state_ = AssetLoadState::Error;
        return false;
    }

    state_ = AssetLoadState::Ready;
    return true;
}

void Texture::CreateResource(Graphics::RenderDevice* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle) {
    // もしリソースが未作成なら、image_ から作成する
    if (!allocation_) {
        if (!image_) return;

        auto metadata = image_->GetMetadata();
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
        desc.Width = static_cast<UINT64>(metadata.width);
        desc.Height = static_cast<UINT>(metadata.height);
        desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
        desc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
        desc.Format = metadata.format;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12MA::ALLOCATION_DESC allocDesc = {};
        allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

        HRESULT hr = device->GetAllocator()->CreateResource(
            &allocDesc,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &allocation_,
            IID_NULL, nullptr
        );
        Assert(SUCCEEDED(hr), "Failed to create Texture resource.");

        // --- データ転送 (Upload) ---
        auto& graphicsEngine = Graphics::GraphicsEngine::GetInstance();
        auto* queue = graphicsEngine.GetCommandQueue();
        auto* currentFrameRes = graphicsEngine.GetCurrentFrameResource();
        
        queue->Reset(currentFrameRes->GetAllocator());
        auto* commandList = queue->GetCommandList();

        // 全サブリソース（MipMap含む）のデータを準備
        std::vector<D3D12_SUBRESOURCE_DATA> subresources(metadata.mipLevels * metadata.arraySize);
        for (size_t i = 0; i < subresources.size(); ++i) {
            const DirectX::Image* img = image_->GetImages() + i;
            subresources[i].pData = img->pixels;
            subresources[i].RowPitch = img->rowPitch;
            subresources[i].SlicePitch = img->slicePitch;
        }

        UINT64 uploadBufferSize = GetRequiredIntermediateSize(allocation_->GetResource(), 0, (UINT)subresources.size());
        ComPtr<D3D12MA::Allocation> uploadAllocation;
        D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
        uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
        auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        device->GetAllocator()->CreateResource(&uploadAllocDesc, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &uploadAllocation, IID_NULL, nullptr);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(allocation_->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        commandList->ResourceBarrier(1, &barrier);
        
        UpdateSubresources(commandList, allocation_->GetResource(), uploadAllocation->GetResource(), 0, 0, (UINT)subresources.size(), subresources.data());
        
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(allocation_->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &barrier);
        
        queue->Execute();
        queue->Wait(queue->Signal());
    }

    auto res = allocation_->GetResource();
    auto desc = res->GetDesc();
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.Format;
    
    // Cubemap 判定
    if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && desc.DepthOrArraySize == 6) {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = desc.MipLevels;
    } else {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
    }
    
    device->GetDevice()->CreateShaderResourceView(res, &srvDesc, srvHandle);
}

bool Texture::CreateFromPixels(Graphics::RenderDevice* device, const uint8_t* pixels, uint32_t width, uint32_t height, DXGI_FORMAT format) {
    // 1. GPUリソースの作成
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->GetAllocator()->CreateResource(
        &allocDesc,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        &allocation_,
        IID_NULL, nullptr
    );
    if (FAILED(hr)) return false;

    // 2. データ転送 (Upload Buffer)
    UINT64 uploadBufferSize = GetRequiredIntermediateSize(allocation_->GetResource(), 0, 1);
    
    ComPtr<D3D12MA::Allocation> uploadAllocation;
    D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
    uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
    auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
    device->GetAllocator()->CreateResource(&uploadAllocDesc, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &uploadAllocation, IID_NULL, nullptr);

    D3D12_SUBRESOURCE_DATA subresource = {};
    subresource.pData = pixels;
    subresource.RowPitch = width * (format == DXGI_FORMAT_R8_UNORM ? 1 : 4);
    subresource.SlicePitch = subresource.RowPitch * height;

    auto& graphicsEngine = Graphics::GraphicsEngine::GetInstance();
    auto* queue = graphicsEngine.GetCommandQueue();
    auto* currentFrameRes = graphicsEngine.GetCurrentFrameResource();
    queue->Reset(currentFrameRes->GetAllocator());
    auto* commandList = queue->GetCommandList();

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(allocation_->GetResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->ResourceBarrier(1, &barrier);
    UpdateSubresources(commandList, allocation_->GetResource(), uploadAllocation->GetResource(), 0, 0, 1, &subresource);
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(allocation_->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);
    queue->Execute();
    queue->Wait(queue->Signal());

    state_ = AssetLoadState::Ready;
    return true;
}

} // namespace Engine::Asset
