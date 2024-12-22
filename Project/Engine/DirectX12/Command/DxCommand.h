#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// dx12のコマンドを管理するクラス
/// ===================================================
class DxCommand {
public:

	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxCommand();
	~DxCommand();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="_dxDevice"> : DxDeviceへのpointer </param>
	void Initialize(class DxDevice* _dxDevice);

	/// <summary>
	/// commandを実行する
	/// </summary>
	void CommandExecute();

	/// <summary>
	/// commandをリセットする
	/// </summary>
	void CommandReset();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12CommandQueue>        commandQueue_     = nullptr;
	ComPtr<ID3D12CommandAllocator>    commandAllocator_ = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList_      = nullptr;

	ComPtr<ID3D12Fence>               fence_            = nullptr;
	uint64_t                          fenceValue_       = 0;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// command queueを取得
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

	/// <summary>
	/// command listを取得
	/// </summary>
	/// <returns> return : command list pointer </returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxCommand(const DxCommand&) = delete;
	DxCommand(DxCommand&&) = delete;
	DxCommand& operator=(const DxCommand&) = delete;
	DxCommand& operator=(DxCommand&&) = delete;
};
