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
    HRESULT hr = DirectX::CreateTexture(device->GetDevice(), metadata, &resource_);
    Assert(SUCCEEDED(hr), "Failed to create Texture resource.");

    // 2. SRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = metadata.format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャとして決め打ち
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
    device->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle);
}

} // namespace Engine::Asset
