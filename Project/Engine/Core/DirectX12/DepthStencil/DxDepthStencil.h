#pragma once

/// directX
#include <d3d12.h>

/// engine
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// depth stencil class
/// ===================================================
class DxDepthStencil final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================
	
	DxDepthStencil();
	~DxDepthStencil();

	/// @brief 初期化処理
	/// @param _dxDevice DxDeviceへのポインタ 
	void Initialize(class DxDevice* _dxDevice, class DxDSVHeap* _dxDsvHeap);


private:
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> depthStencilResource_;


};

