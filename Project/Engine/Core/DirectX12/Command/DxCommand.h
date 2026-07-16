#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"

namespace ONEngine {

/// /////////////////////////////////////////////////
/// dx12のコマンドを管理するクラス
/// /////////////////////////////////////////////////
/**
 * @class DxCommand
 * @brief DirectX12のコマンドキュー、コマンドアロケータ、グラフィックスコマンドリスト、およびGPU同期フェンスを管理するクラス
 */
class DxCommand {
public:
	/// ===================================================
	/// public : method
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	DxCommand();

	/**
	 * @brief デストラクタ。GPUとの同期を待機してからリソースを破棄します。
	 */
	~DxCommand();

	/**
	 * @brief コマンドキュー、アロケータ、グラフィックスコマンドリスト、およびフェンスの生成と初期化を行います。
	 * @param _dxDevice 生成に使用するDxDeviceポインタ
	 */
	void Initialize(class DxDevice* _dxDevice);

	/**
	 * @brief 構築されたコマンドリストを閉じ、コマンドキューに投げて実行します。
	 */
	void CommandExecute();

	/**
	 * @brief コマンドリストをキューで実行し、GPUの処理完了までスレッドを待機（ブロック）させます。
	 */
	void CommandExecuteAndWait();

	/**
	 * @brief 次のフレームの描画命令を記録するため、コマンドアロケータとコマンドリストを再利用可能状態（Reset）にします。
	 */
	void CommandReset();

	/**
	 * @brief フェンスを使用して、GPUがコマンドキューに積まれたすべての処理を終えるまでCPU側スレッドを待機（同期）させます。
	 */
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

	/**
	 * @brief コマンドキュー（ID3D12CommandQueue）オブジェクトを取得します。
	 * @return ID3D12CommandQueueポインタ
	 */
	ID3D12CommandQueue* GetCommandQueue() const;

	/**
	 * @brief コマンドリスト（ID3D12GraphicsCommandList6）オブジェクトを取得します。
	 * @return ID3D12GraphicsCommandList6ポインタ
	 */
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

} /// ONEngine
