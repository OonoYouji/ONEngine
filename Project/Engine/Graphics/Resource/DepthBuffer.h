#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include "D3D12MemAlloc.h"
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

    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHandle_; }

private:
    ComPtr<D3D12MA::Allocation> allocation_;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ = {};
};

} // namespace Engine::Graphics
