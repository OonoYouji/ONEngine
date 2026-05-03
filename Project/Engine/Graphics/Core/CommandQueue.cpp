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
	/// command list
	/// ---------------------------------------------------
	// D3D12では作成時に有効なアロケータが必要なため、一時的なものを作成
	ComPtr<ID3D12CommandAllocator> dummyAllocator;
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&dummyAllocator));
	Assert(SUCCEEDED(result), "Failed to create dummy allocator.");

	result = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		dummyAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	Assert(SUCCEEDED(result), "Failed to create command list.");
	/// コマンドリストは最初は閉じておく
	commandList_->Close();


	/// ---------------------------------------------------
	/// fence
	/// ---------------------------------------------------
	fenceValue_ = 0;
	result = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	Assert(SUCCEEDED(result), "Failed to create fence.");

	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	Assert(fenceEvent_ != nullptr, "Failed to create fence event.");


	/// コマンドリストに名前を付ける
	commandList_->SetName(L"DxCommand CommandList");

	Console::Log("dx command create success!!");
}

void CommandQueue::Shutdown() {
	if (fenceEvent_) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
}

void CommandQueue::Reset(ID3D12CommandAllocator* allocator) {
	allocator->Reset();
	commandList_->Reset(allocator, nullptr);
}

void CommandQueue::Execute() {
	commandList_->Close();
	ID3D12CommandQueue* commandQueue = commandQueue_.Get();
	ID3D12CommandList* ppCommandLists[] = { commandList_.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

uint64_t CommandQueue::Signal() {
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	return fenceValue_;
}

void CommandQueue::Wait(uint64_t fenceValue) {
	if(fence_->GetCompletedValue() < fenceValue) {
		fence_->SetEventOnCompletion(fenceValue, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

ID3D12CommandQueue* CommandQueue::GetCommandQueue() const {
	return commandQueue_.Get();
}

ID3D12GraphicsCommandList6* CommandQueue::GetCommandList() const {
	return commandList_.Get();
}

}