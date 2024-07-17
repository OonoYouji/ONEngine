#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;



namespace ONE {


	/// ===================================================
	/// DirectX12 ダブルバッファ
	/// ===================================================
	class DxDoubleBuffer final {
	public:
		/// ===================================================
		/// public : static objects
		/// ===================================================

		static const int kBufferCount = 2;

	public:


		DxDoubleBuffer();
		~DxDoubleBuffer();

		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// このクラスの初期化
		/// </summary>
		void Initialize(IDXGIFactory7* factory, ID3D12CommandQueue* commandQueue);

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		/// <summary>
		/// SwapChainの初期化
		/// </summary>
		void InitializeSwapChain(IDXGIFactory7* factory,ID3D12CommandQueue* commandQueue);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	private:
		DxDoubleBuffer(const DxDoubleBuffer&) = delete;
		DxDoubleBuffer(DxDoubleBuffer&&) = delete;
		DxDoubleBuffer& operator=(const DxDoubleBuffer&) = delete;
		DxDoubleBuffer& operator=(DxDoubleBuffer&&) = delete;
	};


}