#include "DxRTVHeap.h"

/// engine
#include "Engine/Core/Utility/DebugTools/Log.h"

void DxRTVHeap::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxHeapSize_, false);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	useIndex_       = 0;

	Console::Log("dx descriptor heap rtv create success!!");
}
