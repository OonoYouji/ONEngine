#include "DescriptorHeap.h"

/// engine
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

/// engine::graphics
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine::Graphics {

DescriptorHeap::DescriptorHeap() = default;
DescriptorHeap::~DescriptorHeap() = default;

void DescriptorHeap::Initialize(RenderDevice* renderDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool shaderVisible) {
	HRESULT result = S_FALSE;

	ID3D12Device* device = renderDevice->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = type;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap_));
	Assert(SUCCEEDED(result), "Failed to create descriptor heap.");

	descriptorSize_ = device->GetDescriptorHandleIncrementSize(type);
	startCPUHandle_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	if(shaderVisible) {
		startGPUHandle_ = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	}

    numDescriptors_ = numDescriptors;
    allocatedCount_ = 0;

	Console::Log("dx descriptor heap create success!!");
}

void DescriptorHeap::Shutdown() {
    allocatedCount_ = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(uint32_t index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE handle = startCPUHandle_;
	handle.ptr += static_cast<size_t>(descriptorSize_) * index;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(uint32_t index) const {
	D3D12_GPU_DESCRIPTOR_HANDLE handle = startGPUHandle_;
	handle.ptr += static_cast<size_t>(descriptorSize_) * index;
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate() {
    Assert(allocatedCount_ < numDescriptors_, "Descriptor heap is full!");
    return GetCPUHandle(allocatedCount_++);
}

uint32_t DescriptorHeap::AllocateIndex() {
    Assert(allocatedCount_ < numDescriptors_, "Descriptor heap is full!");
    return allocatedCount_++;
}

} /// namespace Engine::Graphics
