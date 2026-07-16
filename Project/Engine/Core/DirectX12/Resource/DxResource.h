#pragma once

/// directX
#include <d3d12.h>
#include <d3dx12.h>

/// engine
#include "../ComPtr/ComPtr.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector4.h"

/// /////////////////////////////////////////////////
/// ID3D12Resourceのラッパークラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxResource
 * @brief ID3D12Resourceの作成、リソースヒープ（Upload/Default/UAVなど）の確保、レンダーターゲットやUAVテクスチャの初期設定、およびバリア状態遷移をカプセル化するラッパークラス
 */
class DxResource final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	DxResource();

	/**
	 * @brief デストラクタ
	 */
	~DxResource();

	/**
	 * @brief CPUから書き込み可能なアップロードバッファ（定数バッファなど）を生成します。
	 * @param _dxDevice デバイスオブジェクトポインタ
	 * @param _sizeInByte バッファのバイトサイズ
	 */
	void CreateResource(class DxDevice* _dxDevice, size_t _sizeInByte);

	/**
	 * @brief UAV（Unordered Access View）として使用可能なデフォルトヒープ上のバッファを生成します。
	 * @param _dxDevice デバイスポインタ
	 * @param _dxCommand バリアを設定するためのコマンドリスト管理者ポインタ
	 * @param _sizeInByte バイトサイズ
	 */
	void CreateUAVResource(class DxDevice* _dxDevice, class DxCommand* _dxCommand, size_t _sizeInByte);

	/**
	 * @brief GPUのみがアクセス可能なデフォルトヒープ上にバッファを生成します。
	 * @param _dxDevice デバイスポインタ
	 * @param _dxCommand バリアを設定するためのコマンドリスト管理者ポインタ
	 * @param _sizeInByte バイトサイズ
	 * @param _initialState 初期リソース状態
	 */
	void CreateDefaultHeap(class DxDevice* _dxDevice, class DxCommand* _dxCommand, size_t _sizeInByte, D3D12_RESOURCE_STATES _initialState);

	/**
	 * @brief CPUから書き込み可能でGPUからも参照可能なアップロードヒープバッファを生成します。
	 * @param dxDevice デバイスポインタ
	 * @param dxCommand コマンド管理者ポインタ
	 * @param sizeInByte バイトサイズ
	 * @param iniitalState 初期リソース状態
	 */
	void CreateUploadHeap(class DxDevice* dxDevice, class DxCommand* dxCommand, size_t sizeInByte, D3D12_RESOURCE_STATES iniitalState);

	/**
	 * @brief ID3D12Device::CreateCommittedResourceを直接呼び出し、任意のプロパティでバッファを確保します。
	 * @param _dxDevice デバイスオブジェクトへのポインタ
	 * @param _pHeapProperties ヒープ特性設定
	 * @param _HeapFlags ヒープの割り当てフラグ
	 * @param _pDesc リソース定義のディスクリプタ
	 * @param _InitialResourceState 生成時のリソース状態
	 * @param _pOptimizedClearValue レンダーターゲットなどの最適化クリア値
	 */
	void CreateCommittedResource(
		class DxDevice* _dxDevice,
		const D3D12_HEAP_PROPERTIES* _pHeapProperties,
		D3D12_HEAP_FLAGS _HeapFlags,
		const D3D12_RESOURCE_DESC* _pDesc,
		D3D12_RESOURCE_STATES _InitialResourceState,
		const D3D12_CLEAR_VALUE* _pOptimizedClearValue
	);

	/**
	 * @brief レンダーターゲットとして描画可能なテクスチャリソースを生成します。
	 * @param _dxDevice デバイスオブジェクトポインタ
	 * @param _size テクスチャ解像度（幅、高さ）
	 * @param _format 画素フォーマット（DXGI_FORMAT）
	 * @param _clearColor 最適化クリア色（赤、緑、青、アルファ）
	 */
	void CreateRenderTextureResource(
		class DxDevice* _dxDevice,
		const Vector2& _size,
		DXGI_FORMAT _format,
		const Vector4& _clearColor
	);

	/**
	 * @brief コンピュートシェーダ（UAV）から書き込み可能なテクスチャリソースを生成します。
	 * @param _dxDevice デバイスオブジェクトポインタ
	 * @param _size テクスチャ解像度
	 * @param _format 画素フォーマット
	 */
	void CreateUAVTextureResource(
		class DxDevice* _dxDevice,
		const Vector2& _size,
		DXGI_FORMAT _format
	);

	/**
	 * @brief 指定された明示的な状態遷移バリアを作成してコマンドリストに記録し、内部状態を追跡します。
	 * @param _before 遷移前のリソース状態
	 * @param _after 遷移後のリソース状態
	 * @param _dxCommand コマンド管理者ポインタ
	 */
	void CreateBarrier(D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

	/**
	 * @brief 内部で記録している現在の状態から、指定された変更後の状態への遷移バリアを作成し記録します。
	 * @param _after 遷移後のリソース状態
	 * @param _dxCommand コマンド管理者ポインタ
	 */
	void CreateBarrier(D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> resource_ = nullptr;
	D3D12_RESOURCE_STATES  currentState_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief 内部のID3D12Resource生ポインタを取得します。
	 * @return ID3D12Resourceポインタ
	 */
	ID3D12Resource* Get() const;

	/**
	 * @brief 内部のComPtr<ID3D12Resource>参照を取得します。
	 * @return ComPtr参照
	 */
	ComPtr<ID3D12Resource>& GetComPtr();

	/**
	 * @brief 現在追跡しているリソース状態（D3D12_RESOURCE_STATES）を取得します。
	 * @return 現在のリソース状態フラグ
	 */
	D3D12_RESOURCE_STATES GetCurrentState() const;

	/**
	 * @brief 追跡している現在のリソース状態フラグを強制的に上書き設定します（API側で状態が強制変更された場合などに使用）。
	 * @param _state 上書きする状態フラグ
	 */
	void SetCurrentState(D3D12_RESOURCE_STATES _state);

};


/**
 * @brief D3D12デバッグオブジェクトの登録名（SetNameされた文字列）を取得します（デバッグ用）。
 * @param _object 対象D3D12オブジェクトポインタ
 * @return 設定されているワイド文字列
 */
std::wstring GetD3D12Name(ID3D12Object* _object);

/// ===================================================
/// Barrierを作成する関数
/// ===================================================

/**
 * @brief 生のリソースポインタに対して状態遷移（Transition）バリアを記録します。
 * @param _resource 対象リソースポインタ
 * @param _before 遷移前状態
 * @param _after 遷移後状態
 * @param _dxCommand コマンド管理者ポインタ
 */
void CreateBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

/**
 * @brief 複数のDxResourceラッパーオブジェクトに対し、同一の遷移前状態から遷移後状態への一括バリアを適用します。
 * @param _resources 対象DxResourceリスト
 * @param _before 一括遷移前の共通状態
 * @param _after 一括遷移後の共通状態
 * @param _dxCommand コマンド管理者ポインタ
 */
void CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

/**
 * @brief 複数のDxResourceラッパーオブジェクトに対し、それぞれが現在保持している状態から指定の状態への一括遷移バリアを適用します。
 * @param _resources 対象DxResourceリスト
 * @param _after 一括遷移後の共通状態
 * @param _dxCommand コマンド管理者ポインタ
 */
void CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

} /// ONEngine
