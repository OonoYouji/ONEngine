#include "CommandQueue.h"

/// engine
#include "Engine/Common/Assert.h"
#include "Engine/Common/Console.h"

/// engine::graphics
#include "Engine/Graphics/Core/RenderDevice.h"

namespace Engine::Graphics {

CommandQueue::CommandQueue() = default;
CommandQueue::~CommandQueue() = default;

void CommandQueue::Initialize(RenderDevice* renderDevice) {
	HRESULT result = S_FALSE;

	ID3D12Device* device = renderDevice->GetDevice();

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


	/// コマンドリストに名前を付ける
	commandList_->SetName(L"DxCommand CommandList");

	Console::Log("dx command create success!!");
}

void CommandQueue::Shutdown() {

}

ID3D12CommandQueue* CommandQueue::GetCommandQueue() const {
	return commandQueue_.Get();
}

}