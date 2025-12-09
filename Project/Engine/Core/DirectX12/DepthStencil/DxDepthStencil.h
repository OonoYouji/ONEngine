#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"

namespace ONEngine {

/// /////////////////////////////////////////////////
/// depth stencil class
/// /////////////////////////////////////////////////
class DxDepthStencil final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	DxDepthStencil();
	~DxDepthStencil();

	/// @brief 初期化処理
	/// @param _dxDevice  DxDeviceのインスタンス
	/// @param _dxDsvHeap DxDSVHeapのインスタンス
	void Initialize(class DxDevice* _dxDevice, class DxDSVHeap* _dxDsvHeap, class DxSRVHeap* _dxSrvHeap);


	/// @brief リソースの状態をPixelShaderResourceへバリアする
	/// @param _cmdList CommandListのインスタンス
	void CreateBarrierPixelShaderResource(ID3D12GraphicsCommandList* _cmdList);

	/// @brief リソースの状態をDepthWriteへバリアする
	/// @param _cmdList CommandListのインスタンス
	void CreateBarrierDepthWrite(ID3D12GraphicsCommandList* _cmdList);


	/// @brief SRVハンドルの取得
	/// @return SRVハンドルのIndex
	uint32_t GetDepthSrvHandle() const;

	/// @brief DSVハンドルの取得
	/// @return DSVハンドルのIndex
	uint32_t GetDepthDsvHandle() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> depthStencilResource_;
	D3D12_RESOURCE_STATES currentResourceState_;

	uint32_t depthSrvHandle_;
	uint32_t depthDsvHandle_;

};


} /// ONEngine
