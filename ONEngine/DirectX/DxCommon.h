#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace ONE {

	/// ===================================================
	/// DirectX12汎用クラス
	/// ===================================================
	class DxCommon final {
		DxCommon() = default;
		~DxCommon() = default;
	public:

		/// ===================================================
		/// public : methods
		/// ===================================================

		static DxCommon* GetInstance();

		void Initialize();

		void Finalize();

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		void InitializeDevice();

	private:
		
		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
		ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
		ComPtr<ID3D12Device> device_ = nullptr;


	private:
		DxCommon(const DxCommon&) = delete;
		DxCommon(DxCommon&&) = delete;
		DxCommon& operator=(const DxCommon&) = delete;
		DxCommon& operator=(DxCommon&&) = delete;
	};

}