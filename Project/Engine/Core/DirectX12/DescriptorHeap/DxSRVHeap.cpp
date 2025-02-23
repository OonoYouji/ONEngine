#include "DxSRVHeap.h"

/// engine
#include "Engine/Core/Utility/DebugTools/Log.h"

void DxSRVHeap::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxHeapSize_, true);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	bufferHeapSize_ = kMaxHeapSize_ - textureHeapSize_; ///< textureの分を差し引いた分が buffer が使用できるHeapの大きさ

	textureUseIndex_ = 0;
	bufferUseIndex_ = textureHeapSize_; ///< indexの開始地点はtextureの分だけずらす


	Log("dx descriptor heap cbv,srv,uav create success!!");
}


uint32_t DxSRVHeap::AllocateTexture() {

	///< 再利用可能なIndexがある場合
	if (textureSpaceIndex_.size() > 0) {
		uint32_t index = textureSpaceIndex_.front();
		textureSpaceIndex_.pop_front();
		return index;
	}

	++textureUseIndex_;
	return textureUseIndex_ - 1;
}

uint32_t DxSRVHeap::AllocateBuffer() {
	
	///< 再利用可能なIndexがある場合
	if (bufferSpaceIndex_.size() > 0) {
		uint32_t index = bufferSpaceIndex_.front();
		bufferSpaceIndex_.pop_front();
		return index;
	}

	++bufferUseIndex_;
	return bufferUseIndex_ - 1;
}
