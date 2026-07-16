#pragma once

/// directX
#include <d3d12.h>
#include <dxgi1_6.h>
#include "../ComPtr/ComPtr.h"


/// /////////////////////////////////////////////////
/// dx12のデバイスを管理するクラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxDevice
 * @brief DirectX12のメインデバイスおよびDXGIファクトリ・アダプタの生成とライフサイクルを管理するクラス
 */
class DxDevice {
public:
	/// ===================================================
	/// public : method
	/// ===================================================
	
	/**
	 * @brief コンストラクタ
	 */
	DxDevice();

	/**
	 * @brief デストラクタ
	 */
	~DxDevice();
	
	/**
	 * @brief DXGIファクトリの生成、最適なGPU（アダプタ）の選択、D3D12デバイス（ID3D12Device / ID3D12Device10）の初期化、およびデバッグレイヤーの有効化を行います。
	 */
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

	/**
	 * @brief DXGIファクトリ（IDXGIFactory7）を取得します。
	 * @return IDXGIFactory7ポインタ
	 */
	IDXGIFactory7* GetFactory() const;

	/**
	 * @brief D3D12デバイス（ID3D12Device）を取得します。
	 * @return ID3D12Deviceポインタ
	 */
	ID3D12Device* GetDevice() const;

	/**
	 * @brief D3D12デバイス（ID3D12Device10）を取得します。
	 * @return ID3D12Device10ポインタ
	 */
	ID3D12Device10* GetDevice10() const;
};


} /// ONEngine
