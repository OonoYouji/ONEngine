#include "ConstantBuffer.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine::Graphics {

ConstantBuffer::ConstantBuffer() = default;

ConstantBuffer::~ConstantBuffer() {
    if (resource_ && mappedData_) {
        resource_->Unmap(0, nullptr);
    }
}

void ConstantBuffer::Create(RenderDevice* device, uint32_t size) {
    // 256バイトアライメント
    bufferSize_ = (size + 255) & ~255;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize_);

    HRESULT hr = device->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource_)
    );

    Assert(SUCCEEDED(hr), "Failed to create Constant Buffer.");

    // 常時マップ
    hr = resource_->Map(0, nullptr, &mappedData_);
    Assert(SUCCEEDED(hr), "Failed to map Constant Buffer.");
}

void ConstantBuffer::Update(const void* data, uint32_t size) {
    Assert(size <= bufferSize_, "Data size exceeds buffer size.");
    memcpy(mappedData_, data, size);
}

} // namespace Engine::Graphics
