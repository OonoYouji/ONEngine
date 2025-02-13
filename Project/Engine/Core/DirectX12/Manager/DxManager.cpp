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
	dxDescriptorHeaps_[DescriptorHeapType_RTV].reset(new DxDescriptorHeap<DescriptorHeapType_RTV>(dxDevice_.get(), 16));
	dxDescriptorHeaps_[DescriptorHeapType_DSV].reset(new DxDescriptorHeap<DescriptorHeapType_DSV>(dxDevice_.get(), 1));
	dxDescriptorHeaps_[DescriptorHeapType_CBV_SRV_UAV].reset(new DxDescriptorHeap<DescriptorHeapType_CBV_SRV_UAV>(dxDevice_.get(), 128));
	
	for(auto& heap : dxDescriptorHeaps_) {
		heap->Initialize();
	}


	/// depth stencilの初期化
	dxDepthStencil_.reset(new DxDepthStencil());
	dxDepthStencil_->Initialize(dxDevice_.get(), dxDescriptorHeaps_[DescriptorHeapType_DSV].get());

}

void DxManager::Finalize() {}
