#include "DxDSVHeap.h"

/// engine
#include "Engine/Core/Utility/DebugTools/Log.h"

void DxDSVHeap::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxHeapSize_, false);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	useIndex_       = 0;

	Console::Log("dx descriptor heap dsv create success!!");
}
