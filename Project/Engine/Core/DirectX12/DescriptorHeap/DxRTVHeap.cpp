#include "DxRTVHeap.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Utility/Tools/Log.h"

DxRTVHeap::DxRTVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize) : IDxDescriptorHeap(_dxDevice, _maxHeapSize) {}
DxRTVHeap::~DxRTVHeap() = default;

/**
 * @brief RTV用のディスクリプタヒープ（ID3D12DescriptorHeap）をデバイスを通じて新規に生成します。
 */
void DxRTVHeap::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxHeapSize_, false);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	useIndex_ = 0;

	Console::Log("dx descriptor heap rtv create success!!");
}
