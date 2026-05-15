#pragma once

#include <string>
#include <memory>
#include <d3d12.h>
#include "D3D12MemAlloc.h"
#include "Engine/Graphics/Utils/ComPtr.h"
#include "IAsset.h"

namespace DirectX {
    class ScratchImage;
}

namespace Engine::Graphics {
    class RenderDevice;
}


namespace Engine::Asset {

///
/// テクスチャリソースを管理するクラス
///
class Texture : public IAsset {
public:
    Texture();
    virtual ~Texture();

    /// @brief IAssetの実装
    AssetType GetType() const override { return AssetType::Texture; }

    /// @brief ファイルからテクスチャをロード
    bool Load(const std::wstring& filePath);

    /// @brief GPUリソースの作成とSRVの生成
    void CreateResource(Graphics::RenderDevice* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);

    /// @brief メモリ上のピクセルデータからテクスチャを作成
    bool CreateFromPixels(Graphics::RenderDevice* device, const uint8_t* pixels, uint32_t width, uint32_t height, DXGI_FORMAT format);

    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    DirectX::ScratchImage* GetImage() const { return image_.get(); }
    uint32_t GetIndex() const { return index_; }
    void SetIndex(uint32_t index) { index_ = index; }

    uint32_t GetWidth() const { return (uint32_t)GetResource()->GetDesc().Width; }
    uint32_t GetHeight() const { return (uint32_t)GetResource()->GetDesc().Height; }

private:
    std::unique_ptr<DirectX::ScratchImage> image_;
    ComPtr<D3D12MA::Allocation> allocation_;
    uint32_t index_ = 0xFFFFFFFF; // Bindless Index
};

} // namespace Engine::Asset
