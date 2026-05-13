#include "ConstantBuffer.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine::Graphics {

ConstantBuffer::ConstantBuffer() = default;

ConstantBuffer::~ConstantBuffer() {
    if (allocation_ && mappedData_) {
        allocation_->GetResource()->Unmap(0, nullptr);
    }
}

void ConstantBuffer::Create(RenderDevice* device, uint32_t size) {
    // 256バイトアライメント
    bufferSize_ = (size + 255) & ~255;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize_);

    HRESULT hr = device->GetAllocator()->CreateResource(
        &allocDesc,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        &allocation_,
        IID_NULL, nullptr
    );

    Assert(SUCCEEDED(hr), "Failed to create Constant Buffer.");

    // 常時マップ
    hr = allocation_->GetResource()->Map(0, nullptr, &mappedData_);
    Assert(SUCCEEDED(hr), "Failed to map Constant Buffer.");
}

void ConstantBuffer::Update(const void* data, uint32_t size, uint32_t offset) {
    Assert(offset + size <= bufferSize_, "Data out of bounds.");
    memcpy(static_cast<uint8_t*>(mappedData_) + offset, data, size);
}

} // namespace Engine::Graphics
