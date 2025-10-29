#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"


/// /////////////////////////////////////////////////
/// dx12のコマンドを管理するクラス
/// /////////////////////////////////////////////////
class DxCommand {
public:
	/// ===================================================
	/// public : method
	/// ===================================================

	DxCommand();
	~DxCommand();

	/// @brief DxCommandの初期化
	/// @param _dxDevice DxDeviceのインスタンス
	void Initialize(class DxDevice* _dxDevice);

	/// @brief CommandListを実行
	void CommandExecute();

	/// @brief CommandListを実行、GPUの処理を待つ
	void CommandExecuteAndWait();

	/// @brief CommandAllocatorとCommandListをリセットする
	void CommandReset();

	/// @brief GPUの処理が完了するまで待機する
	void WaitForGpuComplete();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12CommandQueue>         commandQueue_;
	ComPtr<ID3D12CommandAllocator>     commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList6> commandList_;

	ComPtr<ID3D12Fence>                fence_;
	uint64_t                           fenceValue_;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief CommandQueueの取得
	ID3D12CommandQueue* GetCommandQueue() const;

	/// @brief CommandListの取得
	ID3D12GraphicsCommandList6* GetCommandList() const;


private:
	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxCommand(const DxCommand&) = delete;
	DxCommand(DxCommand&&) = delete;
	DxCommand& operator=(const DxCommand&) = delete;
	DxCommand& operator=(DxCommand&&) = delete;
};
