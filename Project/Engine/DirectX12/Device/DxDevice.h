#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../ComPtr/ComPtr.h"

class DxDevice {
public:

	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxDevice();
	~DxDevice();
	
	void Initialize();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_  = nullptr;
	ComPtr<ID3D12Device>  device_      = nullptr;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// deviceを取得
	/// </summary>
	/// <returns></returns>
	inline ID3D12Device* GetDevice() const { return device_.Get(); }

};

