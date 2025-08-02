#include "DxDebug.h"

#include "../Device/DxDevice.h"

DxDebug::DxDebug() {}
DxDebug::~DxDebug() {}

void DxDebug::Initialize(DxDevice* _dxDevice) {
#ifdef DEBUG_MODE
	ComPtr<ID3D12InfoQueue> infoQueue;
	if(SUCCEEDED(_dxDevice->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severtities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs        = _countof(denyIds);
		filter.DenyList.pIDList       = denyIds;
		filter.DenyList.NumSeverities = _countof(severtities);
		filter.DenyList.pSeverityList = severtities;

		infoQueue->PushStorageFilter(&filter);
		infoQueue.Reset();
	}
#endif // DEBUG_MODE
}


void DxDebug::SetDebugLayer() {
#ifdef DEBUG_MODE
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif // DEBUG_MODE
}