#pragma once

#include <d3d12.h>
#include <vector>
#include <cstdint>
#include "D3D12MemAlloc.h"
#include "Engine/Graphics/Utils/ComPtr.h"

namespace Engine::Graphics {

class RenderDevice;
class CommandQueue;

///
/// 構造化バッファ（頂点データなどに使用）
///
class StructuredBuffer {
public:
    StructuredBuffer();
    ~StructuredBuffer();

    void Create(RenderDevice* device, uint32_t stride, uint32_t count, const void* initialData = nullptr);
    void Update(const void* data, uint32_t size);
    
    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    uint32_t GetStride() const { return stride_; }
    uint32_t GetCount() const { return count_; }

private:
    ComPtr<D3D12MA::Allocation> allocation_;
    uint32_t stride_ = 0;
    uint32_t count_ = 0;
};

///
/// インデックスバッファ
///
class IndexBuffer {
public:
    IndexBuffer();
    ~IndexBuffer();

    void Create(RenderDevice* device, uint32_t count, const uint32_t* initialData = nullptr);

    ID3D12Resource* GetResource() const { return allocation_->GetResource(); }
    D3D12_INDEX_BUFFER_VIEW GetView() const { return view_; }
    uint32_t GetCount() const { return count_; }

private:
    ComPtr<D3D12MA::Allocation> allocation_;
    D3D12_INDEX_BUFFER_VIEW view_ = {};
    uint32_t count_ = 0;
};

} // namespace Engine::Graphics
