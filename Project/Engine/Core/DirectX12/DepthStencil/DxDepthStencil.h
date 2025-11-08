#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>

/// engine
#include "../ComPtr/ComPtr.h"


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


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> depthStencilResource_;
	uint32_t depthSrvHandle_;
};

