#include "GpuBuffer.h"
#include <d3dx12.h>
#include "Engine/Graphics/Core/RenderDevice.h"
#include "Engine/Common/Assert.h"

namespace Engine::Graphics {

// --- StructuredBuffer ---

StructuredBuffer::StructuredBuffer() = default;
StructuredBuffer::~StructuredBuffer() = default;

void StructuredBuffer::Create(RenderDevice* device, uint32_t stride, uint32_t count, const void* initialData, bool isUAV) {
    stride_ = stride;
    count_ = count;
    isUAV_ = isUAV;
    uint32_t totalSize = stride * count;

    D3D12MA::ALLOCATION_DESC allocDesc = {};
    allocDesc.HeapType = isUAV ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_FLAGS flags = isUAV ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize, flags);

    // DEFAULTヒープの場合は COMMON (あるいは UAV なら最初から UAV でもいいが、COMMON が無難)
    // UPLOADヒープの場合は GENERIC_READ 固定
    D3D12_RESOURCE_STATES initialState = isUAV ? D3D12_RESOURCE_STATE_COMMON : D3D12_RESOURCE_STATE_GENERIC_READ;

    HRESULT hr = device->GetAllocator()->CreateResource(
        &allocDesc,
        &resourceDesc,
        initialState,
        nullptr,
        &allocation_,
        IID_NULL, nullptr
    );

    Assert(SUCCEEDED(hr), "Failed to create Structured Buffer.");
    currentState_ = initialState;

    if (initialData) {
        if (isUAV) {
            // UAVの場合は別途ステージングバッファ経由でアップロードが必要。
            // 現時点では初期データなしを想定するか、ParticleSystem 側で初期化を行う。
            Assert(false, "Initial data for UAV StructuredBuffer is not supported in Create. Use a staging buffer.");
        } else {
            Update(initialData, totalSize, 0);
        }
    }
}

void StructuredBuffer::Transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES stateAfter) {
    if (!allocation_ || currentState_ == stateAfter) return;
    
    // UPLOADヒープの場合は遷移させてはいけない (常に GENERIC_READ)
    if (!isUAV_) {
        currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
        return;
    }

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(allocation_->GetResource(), currentState_, stateAfter);
    commandList->ResourceBarrier(1, &barrier);
    currentState_ = stateAfter;
}


void StructuredBuffer::Update(const void* data, uint32_t size, uint32_t offset) {
    if (!allocation_ || !data) return;
    
    // UPLOADヒープのみ Map 可能
    Assert(!isUAV_, "Cannot Update a UAV StructuredBuffer directly from CPU. It's in DEFAULT heap.");

    void* mapped = nullptr;
    allocation_->GetResource()->Map(0, nullptr, &mapped);
    char* dest = static_cast<char*>(mapped) + offset;
    memcpy(dest, data, size);
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
        Update(initialData, count, 0);
    }

    view_.BufferLocation = allocation_->GetResource()->GetGPUVirtualAddress();
    view_.SizeInBytes = totalSize;
    view_.Format = DXGI_FORMAT_R32_UINT;
}

void IndexBuffer::Update(const uint32_t* data, uint32_t count, uint32_t offset) {
    if (!allocation_ || !data) return;
    void* mapped = nullptr;
    allocation_->GetResource()->Map(0, nullptr, &mapped);
    uint32_t* dest = static_cast<uint32_t*>(mapped) + offset;
    memcpy(dest, data, count * sizeof(uint32_t));
    allocation_->GetResource()->Unmap(0, nullptr);
}

} // namespace Engine::Graphics
