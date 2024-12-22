#include "DxCommand.h"

/// engine
#include "../Device/DxDevice.h"

/// lib
#include "Lib/DebugTools/Assert.h"
#include "Lib/DebugTools/Log.h"


DxCommand::DxCommand() {}
DxCommand::~DxCommand() {}


void DxCommand::Initialize(DxDevice* _dxDevice) {
	HRESULT hr = S_FALSE;

	ID3D12Device* device = _dxDevice->GetDevice();

	/// ---------------------------------------------------
	/// command queue
	/// ---------------------------------------------------
	D3D12_COMMAND_QUEUE_DESC desc{};
	hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
	Assert(SUCCEEDED(hr), "Failed to create command queue.");



	/// ---------------------------------------------------
	/// command allocator
	/// ---------------------------------------------------
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	Assert(SUCCEEDED(hr), "Failed to create command allocator.");


	/// ---------------------------------------------------
	/// command list
	/// ---------------------------------------------------
	hr = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	Assert(SUCCEEDED(hr), "Failed to create command list.");


	/// ---------------------------------------------------
	/// fence
	/// ---------------------------------------------------

	fenceValue_ = 0;
	hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	Assert(SUCCEEDED(hr), "Failed to create fence.");


	Log("dx command create success!!");
}

