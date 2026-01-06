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

class DxResource final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DxResource();
	~DxResource();

	/// @brief bufferを作成する
	/// @param _dxDevice デバイスオブジェクトへのポインタ
	/// @param _sizeInByte バッファのサイズ（バイト単位）
	void CreateResource(class DxDevice* _dxDevice, size_t _sizeInByte);
	void CreateUAVResource(class DxDevice* _dxDevice, class DxCommand* _dxCommand, size_t _sizeInByte);
	void CreateDefaultHeap(class DxDevice* _dxDevice, class DxCommand* _dxCommand, size_t _sizeInByte, D3D12_RESOURCE_STATES _initialState);

	/// @brief resourceを作成する
	/// @param _dxDevice デバイスオブジェクトへのポインタ
	/// @param _pHeapProperties ヒーププロパティ
	/// @param _HeapFlags ヒープフラグ
	/// @param _pDesc リソースの記述子
	/// @param _InitialResourceState 初期リソース状態
	/// @param _pOptimizedClearValue 最適化されたクリア値
	void CreateCommittedResource(
		class DxDevice* _dxDevice,
		const D3D12_HEAP_PROPERTIES* _pHeapProperties,
		D3D12_HEAP_FLAGS _HeapFlags,
		const D3D12_RESOURCE_DESC* _pDesc,
		D3D12_RESOURCE_STATES _InitialResourceState,
		const D3D12_CLEAR_VALUE* _pOptimizedClearValue
	);

	/// @brief render texture resourceを作成する
	/// @param _dxDevice デバイスオブジェクトへのポインタ
	/// @param _size render textureのサイズ
	/// @param _format 書き込みフォーマット
	/// @param _clearColor クリアカラー
	void CreateRenderTextureResource(
		class DxDevice* _dxDevice,
		const Vector2& _size,
		DXGI_FORMAT _format,
		const Vector4& _clearColor
	);

	/// @brief unordered access view texture resourceを作成する
	/// @param _dxDevice DxDeviceのインスタンスへのポインタ
	/// @param _size texture size
	/// @param _format dxgi format
	void CreateUAVTextureResource(
		class DxDevice* _dxDevice,
		const Vector2& _size,
		DXGI_FORMAT _format
	);

	/// @brief リソースのバリアーを作成する
	/// @param _before 前の状態
	/// @param _after 変更後の状態
	/// @param _dxCommand DxCommandのインスタンスへのポインタ
	void CreateBarrier(D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);
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

	/// @brief リソースオブジェクトを取得する
	/// @return ID3D12Resourceオブジェクトへのポインタ
	ID3D12Resource* Get() const;
	ComPtr<ID3D12Resource>& GetComPtr();

	/// @brief 現在のリソース状態を取得する
	/// @return リソース状態
	D3D12_RESOURCE_STATES GetCurrentState() const;

	/// @brief 現在のステートを変更する(UAVの作成など強制的に状態が変更される場合のみ使用する)
	/// @param _state 変更先のステート
	void SetCurrentState(D3D12_RESOURCE_STATES _state);

};


std::wstring GetD3D12Name(ID3D12Object* _object);

/// ===================================================
/// Barrierを作成する関数
/// ===================================================
void CreateBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

void CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

void CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _after, class DxCommand* _dxCommand);

} /// ONEngine
