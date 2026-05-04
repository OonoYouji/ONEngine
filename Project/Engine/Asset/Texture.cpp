#include "Texture.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include <filesystem>
#include "Engine/Graphics/Core/RenderDevice.h"
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
    if (!image_) return;

    auto metadata = image_->GetMetadata();
    
    // 1. GPUリソースの作成
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
    desc.Width = static_cast<UINT64>(metadata.width);
    desc.Height = static_cast<UINT>(metadata.height);
    desc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
    desc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
    desc.Format = metadata.format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
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

    // 2. SRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = metadata.format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして決め打ち
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
    device->GetDevice()->CreateShaderResourceView(allocation_->GetResource(), &srvDesc, srvHandle);
}

} // namespace Engine::Asset
