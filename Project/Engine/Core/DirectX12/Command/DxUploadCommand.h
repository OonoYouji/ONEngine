#pragma once

/// directX
#include <d3d12.h>
#include <windows.h> /// [追加] HANDLE用

/// engine
#include "../ComPtr/ComPtr.h"

namespace ONEngine {

class DxDevice;

/// ===================================================
/// 並列ロード専用コマンドクラス
/// - 各スレッドで使用可能
/// - Queue は共有
/// - 各スレッドが安全に待機できるよう専用Fenceを保持 [改変]
/// ===================================================
class DxUploadCommand {
public:
	DxUploadCommand();
	~DxUploadCommand();

	/// 初期化（スレッドごとに1回）
	void Initialize(DxDevice* device);

	/// 記録開始
	void Begin();

	/// 記録終了
	void End();

	/// [追加] 共有のQueueにコマンドを投げ、自身のFenceで完了を待機する
	void ExecuteAndWait(ID3D12CommandQueue* commandQueue);

	/// コマンドリスト取得
	ID3D12GraphicsCommandList6* GetCommandList() const;

private:
	ComPtr<ID3D12CommandAllocator>     allocator_;
	ComPtr<ID3D12GraphicsCommandList6> commandList_;

	/// [追加] スレッド待機用フェンス
	ComPtr<ID3D12Fence> fence_;
	UINT64              fenceValue_;
	HANDLE              fenceEvent_;

private:
	DxUploadCommand(const DxUploadCommand&) = delete;
	DxUploadCommand& operator=(const DxUploadCommand&) = delete;
};

} // namespace ONEngine