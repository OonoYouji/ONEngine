#include "DxCommand.h"

/// engine
#include "../Device/DxDevice.h"

/// lib
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"

using namespace ONEngine;

DxCommand::DxCommand()
	: commandQueue_(nullptr),
	commandAllocator_(nullptr),
	commandList_(nullptr),
	fence_(nullptr),
	fenceValue_(0) {
};

DxCommand::~DxCommand() = default;


/**
 * @brief コマンドキュー、アロケータ、グラフィックスコマンドリスト、およびフェンスの生成と初期化を行います。
 */
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


	/// コマンドリストに名前を付ける
	commandList_->SetName(L"DxCommand CommandList");

	Console::Log("dx command create success!!");
}

/**
 * @brief 構築されたコマンドリストを閉じ、コマンドキューに投げて実行します。
 */
void DxCommand::CommandExecute() {
	/// ----- CommandListのClose、Execute、Wait ----- ///

	HRESULT hr = commandList_->Close();
	Assert(SUCCEEDED(hr), "Failed to close command list.");

	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

/**
 * @brief コマンドリストをキューで実行し、GPUの処理完了までスレッドを待機（ブロック）させます。
 */
void DxCommand::CommandExecuteAndWait() {
	CommandExecute();
	WaitForGpuComplete();
}

/**
 * @brief 次のフレームの描画命令を記録するため、コマンドアロケータとコマンドリストを再利用可能状態にします。
 */
void DxCommand::CommandReset() {
	/// ----- CommandAllocatorとCommandListのReset ----- ///

	HRESULT hr = commandAllocator_->Reset();
	Assert(SUCCEEDED(hr), "command allocator reset failed.");

	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	Assert(SUCCEEDED(hr), "Failed to reset command list.");
}

/**
 * @brief フェンスを使用して、GPUがコマンドキューに積まれたすべての処理を終えるまでCPU側スレッドを待機させます。
 */
void DxCommand::WaitForGpuComplete() {
	/// ----- GPUの処理を待つ ----- ///

	++fenceValue_;
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	if (fence_->GetCompletedValue() < fenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

/**
 * @brief コマンドキュー（ID3D12CommandQueue）オブジェクトを取得します。
 */
ID3D12CommandQueue* DxCommand::GetCommandQueue() const {
	return commandQueue_.Get();
}

/**
 * @brief コマンドリスト（ID3D12GraphicsCommandList6）オブジェクトを取得します。
 */
ID3D12GraphicsCommandList6* DxCommand::GetCommandList() const {
	return commandList_.Get();
}

