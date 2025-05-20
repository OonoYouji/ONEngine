#include "DxManager.h"

DxManager::DxManager() {}

DxManager::~DxManager() {}

void DxManager::Initialize() {

	/// deug layerをセット
	dxDebug_.reset(new DxDebug());
	dxDebug_->SetDebugLayer();


	/// deviceの初期化
	dxDevice_.reset(new DxDevice());
	dxDevice_->Initialize();


	/// debugの初期化
	dxDebug_->Initialize(dxDevice_.get());


	/// commandの初期化
	dxCommand_.reset(new DxCommand());
	dxCommand_->Initialize(dxDevice_.get());

	
	/// descriptor heapの初期化
	dxDescriptorHeaps_[DescriptorHeapType_RTV].reset(new DxRTVHeap(dxDevice_.get(), 16));
	dxDescriptorHeaps_[DescriptorHeapType_DSV].reset(new DxDSVHeap(dxDevice_.get(), 1));
	dxDescriptorHeaps_[DescriptorHeapType_CBV_SRV_UAV].reset(new DxSRVHeap(dxDevice_.get(), 128, 32));
	
	for(auto& heap : dxDescriptorHeaps_) {
		heap->Initialize();
	}


	/// depth stencilの初期化
	dxDepthStencil_.reset(new DxDepthStencil());
	dxDepthStencil_->Initialize(dxDevice_.get(), GetDxDSVHeap());

}

void DxManager::Finalize() {}



DxDevice* DxManager::GetDxDevice() const {
	return dxDevice_.get();
}

DxCommand* DxManager::GetDxCommand() const {
	return dxCommand_.get();
}

DxSRVHeap* DxManager::GetDxSRVHeap() const {
	return static_cast<DxSRVHeap*>(dxDescriptorHeaps_[DescriptorHeapType_CBV_SRV_UAV].get());
}

DxRTVHeap* DxManager::GetDxRTVHeap() const {
	return static_cast<DxRTVHeap*>(dxDescriptorHeaps_[DescriptorHeapType_RTV].get());
}

DxDSVHeap* DxManager::GetDxDSVHeap() const {
	return static_cast<DxDSVHeap*>(dxDescriptorHeaps_[DescriptorHeapType_DSV].get());
}
