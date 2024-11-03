#include "DxCommand.h"

#include <cassert>


ONE::DxCommand::DxCommand() {}
ONE::DxCommand::~DxCommand() {
	list_.Reset();
	allocator_.Reset();
	queue_.Reset();
}


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxCommand::Initialize(ID3D12Device* device) {
	HRESULT hr = S_FALSE;

	/// ---------------------------------------------------
	/// command queue
	/// ---------------------------------------------------
	D3D12_COMMAND_QUEUE_DESC desc{};
	hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue_));
	assert(SUCCEEDED(hr));



	/// ---------------------------------------------------
	/// command allocator
	/// ---------------------------------------------------
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
	assert(SUCCEEDED(hr));


	/// ---------------------------------------------------
	/// command list
	/// ---------------------------------------------------
	hr = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		allocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&list_)
	);
	assert(SUCCEEDED(hr));


	InitializeFence(device);


}

void ONE::DxCommand::Close() {
	HRESULT hr = list_->Close();
	assert(SUCCEEDED(hr));
}

void ONE::DxCommand::Execution() {
	ID3D12CommandList* commandLists[] = { list_.Get() };
	queue_->ExecuteCommandLists(1, commandLists);

	++fenceValue_;
	queue_->Signal(fence_.Get(), fenceValue_);

	if(fence_->GetCompletedValue() < fenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

}

void ONE::DxCommand::Reset() {
	HRESULT hr = allocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = list_->Reset(allocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

bool ONE::DxCommand::IsExeCommandEnded() {
	++fenceValue_;
	queue_->Signal(fence_.Get(), fenceValue_);

	if(fence_->GetCompletedValue() < fenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	return true;
}

void ONE::DxCommand::InitializeFence(ID3D12Device* device) {

	fenceValue_ = 0;
	HRESULT hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

}



