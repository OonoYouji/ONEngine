#pragma once

#include <string>
#include <memory>
#include <d3d12.h>
#include "Engine/Graphics/Utils/ComPtr.h"

namespace DirectX {
    class ScratchImage;
}

namespace Engine::Graphics {

class RenderDevice;

///
/// テクスチャリソースを管理するクラス
///
class Texture {
public:
    Texture();
    ~Texture();

    /// @brief ファイルからテクスチャをロード
    bool Load(const std::wstring& filePath);

    /// @brief GPUリソースの作成とSRVの生成
    void CreateResource(RenderDevice* device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);

    ID3D12Resource* GetResource() const { return resource_.Get(); }
    DirectX::ScratchImage* GetImage() const { return image_.get(); }
    uint32_t GetIndex() const { return index_; }
    void SetIndex(uint32_t index) { index_ = index; }

private:
    std::unique_ptr<DirectX::ScratchImage> image_;
    ComPtr<ID3D12Resource> resource_;
    uint32_t index_ = 0xFFFFFFFF; // Bindless Index
};

} // namespace Engine::Graphics
