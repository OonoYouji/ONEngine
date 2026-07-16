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
/**
 * @class DxUploadCommand
 * @brief マルチスレッドロード（アップロード）処理で各スレッドが個別に保持する専用のコマンドバッファ（コマンドアロケータ、コマンドリスト、同期フェンス）を管理するクラス
 */
class DxUploadCommand {
public:
	/**
	 * @brief コンストラクタ
	 */
	DxUploadCommand();

	/**
	 * @brief デストラクタ
	 */
	~DxUploadCommand();

	/**
	 * @brief アロケータ、コマンドリスト、およびスレッド待機用の専用フェンス・イベントを初期化します。
	 * @param device 初期化に使用するDxDeviceポインタ
	 */
	void Initialize(DxDevice* device);

	/**
	 * @brief コマンド記録のためにアロケータおよびコマンドリストをリセットして開始します。
	 */
	void Begin();

	/**
	 * @brief コマンド記録を終了（Close）します。
	 */
	void End();

	/**
	 * @brief 記録したコマンドリストを指定されたコマンドキューに送信し、このスレッド用のフェンスで完了するまでCPU側を待機させます。
	 * @param commandQueue 実行先の共有コマンドキュー
	 */
	void ExecuteAndWait(ID3D12CommandQueue* commandQueue);

	/**
	 * @brief 内部のコマンドリストを取得します。
	 * @return ID3D12GraphicsCommandList6ポインタ
	 */
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