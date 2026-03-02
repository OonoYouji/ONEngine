#include "DxUploadCommand.h"

/// engine
#include "../Device/DxDevice.h"

namespace ONEngine {

DxUploadCommand::DxUploadCommand()
	: fenceValue_(0), fenceEvent_(nullptr) {
}

DxUploadCommand::~DxUploadCommand() {
	if(fenceEvent_) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
}

void DxUploadCommand::Initialize(DxDevice* device) {
	ID3D12Device* d3dDevice = device->GetDevice();

	// Allocator
	d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&allocator_)
	);

	// CommandList
	d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		allocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);

	// 初期状態はCloseしておく
	commandList_->Close();

	// [追加] 待機用のFenceとイベントの作成
	d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void DxUploadCommand::Begin() {
	allocator_->Reset();
	commandList_->Reset(allocator_.Get(), nullptr);
}

void DxUploadCommand::End() {
	commandList_->Close();
}

void DxUploadCommand::ExecuteAndWait(ID3D12CommandQueue* commandQueue) {
	// コマンドの実行 (ID3D12CommandQueue自体はスレッドセーフなのでロック不要)
	ID3D12CommandList* lists[] = { commandList_.Get() };
	commandQueue->ExecuteCommandLists(1, lists);

	// シグナルを送信して待機
	fenceValue_++;
	commandQueue->Signal(fence_.Get(), fenceValue_);

	if(fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

ID3D12GraphicsCommandList6* DxUploadCommand::GetCommandList() const {
	return commandList_.Get();
}

} // namespace ONEngine