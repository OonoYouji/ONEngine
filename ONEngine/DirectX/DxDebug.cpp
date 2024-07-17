#include <DxDebug.h>


ONE::DxDebug::DxDebug() {}
ONE::DxDebug::~DxDebug() {
#ifdef _DEBUG
	debugController_.Reset();
#endif // _DEBUG

}


void ONE::DxDebug::Initialize(ID3D12Device* device) {
#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severtities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severtities);
		filter.DenyList.pSeverityList = severtities;
		
		infoQueue->PushStorageFilter(&filter);
		infoQueue.Reset();
	}

#endif // _DEBUG
}

void ONE::DxDebug::SetDebugLayer() {
#ifdef _DEBUG
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG
}