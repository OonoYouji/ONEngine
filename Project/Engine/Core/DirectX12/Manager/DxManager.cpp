#include "DxManager.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DescriptorHeapSize.h"

DxManager::DxManager() = default;
DxManager::~DxManager() = default;

void DxManager::Initialize() {

	/// deug layerをセット
	dxDebug_ = std::make_unique<DxDebug>();
	dxDebug_->SetDebugLayer();


	/// deviceの初期化
	dxDevice_ = std::make_unique<DxDevice>();
	dxDevice_->Initialize();


	/// debugの初期化
	dxDebug_->Initialize(dxDevice_.get());


	/// commandの初期化
	dxCommand_ = std::make_unique<DxCommand>();
	dxCommand_->Initialize(dxDevice_.get());


	/// descriptor heapの初期化
	dxDescriptorHeaps_[DescriptorHeapType_RTV] = std::make_unique<DxRTVHeap>(dxDevice_.get(), DescriptorHeapLimits::RTV);
	dxDescriptorHeaps_[DescriptorHeapType_DSV] = std::make_unique<DxDSVHeap>(dxDevice_.get(), DescriptorHeapLimits::DSV);
	dxDescriptorHeaps_[DescriptorHeapType_CBV_SRV_UAV] = std::make_unique<DxSRVHeap>(dxDevice_.get(), DescriptorHeapLimits::CBV_SRV_UAV, MAX_TEXTURE_COUNT);

	for (auto& heap : dxDescriptorHeaps_) {
		heap->Initialize();
	}


	/// depth stencilの初期化
	dxDepthStencil_.reset(new DxDepthStencil());
	dxDepthStencil_->Initialize(dxDevice_.get(), GetDxDSVHeap());

}

void DxManager::HeapBindToCommandList() {
	GetDxSRVHeap()->BindToCommandList(
		GetDxCommand()->GetCommandList()
	);
}



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
