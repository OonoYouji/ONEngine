#include "GpuBuffer.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"

namespace Engine::Graphics {

// --- StructuredBuffer ---

StructuredBuffer::StructuredBuffer() = default;
StructuredBuffer::~StructuredBuffer() = default;

void StructuredBuffer::Create(RenderDevice* device, uint32_t stride, uint32_t count, const void* initialData) {
    stride_ = stride;
    count_ = count;
    uint32_t totalSize = stride * count;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

    HRESULT hr = device->GetAllocator()->CreateResource(
        &allocDesc,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &allocation_,
        IID_NULL, nullptr
    );

    Assert(SUCCEEDED(hr), "Failed to create Structured Buffer.");

    if (initialData) {
        Update(initialData, totalSize);
    }
}

void StructuredBuffer::Update(const void* data, uint32_t size) {
    if (!allocation_ || !data) return;
    void* mapped = nullptr;
    allocation_->GetResource()->Map(0, nullptr, &mapped);
    memcpy(mapped, data, size);
    allocation_->GetResource()->Unmap(0, nullptr);
}

// --- IndexBuffer ---

IndexBuffer::IndexBuffer() = default;
IndexBuffer::~IndexBuffer() = default;

void IndexBuffer::Create(RenderDevice* device, uint32_t count, const uint32_t* initialData) {
    count_ = count;
    uint32_t totalSize = sizeof(uint32_t) * count;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

    HRESULT hr = device->GetAllocator()->CreateResource(
        &allocDesc,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &allocation_,
        IID_NULL, nullptr
    );

    Assert(SUCCEEDED(hr), "Failed to create Index Buffer.");

    if (initialData) {
        void* mapped = nullptr;
        allocation_->GetResource()->Map(0, nullptr, &mapped);
        memcpy(mapped, initialData, totalSize);
        allocation_->GetResource()->Unmap(0, nullptr);
    }

    view_.BufferLocation = allocation_->GetResource()->GetGPUVirtualAddress();
    view_.SizeInBytes = totalSize;
    view_.Format = DXGI_FORMAT_R32_UINT;
}

} // namespace Engine::Graphics
