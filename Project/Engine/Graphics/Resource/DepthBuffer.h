#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {

class RenderDevice;
class DescriptorHeap;

///
/// 深度バッファを管理するクラス
///
class DepthBuffer {
public:
    DepthBuffer();
    ~DepthBuffer();

    /// @brief 深度バッファの作成
    void Create(RenderDevice* device, DescriptorHeap* dsvHeap, uint32_t width, uint32_t height);

    ID3D12Resource* GetResource() const { return resource_.Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHandle_; }

private:
    ComPtr<ID3D12Resource> resource_;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ = {};
};

} // namespace Engine::Graphics
