#include "DxCommand.h"

/// engine
#include "../Device/DxDevice.h"

/// lib
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Core/Utility/DebugTools/Log.h"


DxCommand::DxCommand() {}
DxCommand::~DxCommand() {}


void DxCommand::Initialize(DxDevice* _dxDevice) {
	HRESULT result = S_FALSE;

	ID3D12Device* device = _dxDevice->GetDevice();

	/// ---------------------------------------------------
	/// command queue
	/// ---------------------------------------------------
	D3D12_COMMAND_QUEUE_DESC desc{};
	result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
	Assert(SUCCEEDED(result), "Failed to create command queue.");



	/// ---------------------------------------------------
	/// command allocator
	/// ---------------------------------------------------
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	Assert(SUCCEEDED(result), "Failed to create command allocator.");


	/// ---------------------------------------------------
	/// command list
	/// ---------------------------------------------------
	result = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	Assert(SUCCEEDED(result), "Failed to create command list.");


	/// ---------------------------------------------------
	/// fence
	/// ---------------------------------------------------

	fenceValue_ = 0;
	result = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	Assert(SUCCEEDED(result), "Failed to create fence.");


	Console::Log("dx command create success!!");
}

void DxCommand::CommandExecute() {
	HRESULT hr = commandList_->Close();
	Assert(SUCCEEDED(hr), "Failed to close command list.");

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	WaitForGpuComplete();
}

void DxCommand::CommandReset() {
	HRESULT hr = commandAllocator_->Reset();
	Assert(SUCCEEDED(hr), "command allocator reset failed.");

	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	Assert(SUCCEEDED(hr), "Failed to reset command list.");
}

void DxCommand::WaitForGpuComplete() {
	++fenceValue_;
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	if(fence_->GetCompletedValue() < fenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

