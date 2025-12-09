#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../ComPtr/ComPtr.h"


/// /////////////////////////////////////////////////
/// dx12のデバイスを管理するクラス
/// /////////////////////////////////////////////////
class DxDevice {
public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	DxDevice();
	~DxDevice();
	
	/// @brief 初期化
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

	/// @brief DxgiFactoryのインスタンスの取得
	/// @return DxgiFactoryインスタンス
	IDXGIFactory7* GetFactory() const;

	/// @brief Deviceのインスタンスの取得
	/// @return Deviceインスタンス
	ID3D12Device* GetDevice() const;

	/// @brief Device10のインスタンスの取得
	/// @return Device10インスタンス
	ID3D12Device10* GetDevice10() const;
};

