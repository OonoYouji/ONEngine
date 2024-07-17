#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace ONE {

	/// ===================================================
	/// DirectX12 Deviceの管理
	/// ===================================================
	class DxDevice final {
	public:

		DxDevice();
		~DxDevice();


		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// Deviceのゲッタ
		/// </summary>
		/// <returns></returns>
		ID3D12Device* GetDevice() const { return device_.Get(); }

		/// <summary>
		/// Factoryのゲッタ
		/// </summary>
		/// <returns></returns>
		IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
		ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
		ComPtr<ID3D12Device> device_ = nullptr;

	private:
		DxDevice(const DxDevice&) = delete;
		DxDevice(DxDevice&&) = delete;
		DxDevice& operator= (const DxDevice&) = delete;
		DxDevice& operator= (DxDevice&&) = delete;
	};

}