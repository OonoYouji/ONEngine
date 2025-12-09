#include "DxDSVHeap.h"

/// engine
#include "Engine/Core/Utility/Tools/Log.h"

DxDSVHeap::DxDSVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize) : IDxDescriptorHeap(_dxDevice, _maxHeapSize) {}
DxDSVHeap::~DxDSVHeap() = default;

void DxDSVHeap::Initialize() {
	ID3D12Device* device = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxHeapSize_, false);
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	useIndex_       = 0;

	Console::Log("dx descriptor heap dsv create success!!");
}
