#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../ComPtr/ComPtr.h"


/// ===================================================
/// dx12のデバイスを管理するクラス
/// ===================================================
class DxDevice {
public:

	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxDevice();
	~DxDevice();
	
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_  = nullptr;
	ComPtr<ID3D12Device>  device_      = nullptr;
	ComPtr<ID3D12Device10> device10_ = nullptr;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// factoryを取得
	/// </summary>
	/// <returns></returns>
	IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

	/// <summary>
	/// deviceを取得
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	ID3D12Device10* GetDevice10() const { return device10_.Get(); }

};

