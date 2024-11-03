#pragma once

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;


namespace ONE {

	/// ===================================================
	/// DirectX : デプスステンシル
	/// ===================================================
	class DxDepthStencil final {
	public:
		DxDepthStencil();
		~DxDepthStencil();
		
		/// ===================================================
		/// public : methods
		/// ===================================================

		void Initialize(ID3D12Device* device);

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		void CreateDepthStencil(ID3D12Device* device);

		void CreateView(ID3D12Device* device);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	private:
		DxDepthStencil(const DxDepthStencil&) = delete;
		DxDepthStencil(DxDepthStencil&&) = delete;
		DxDepthStencil& operator=(const DxDepthStencil&) = delete;
		DxDepthStencil& operator=(DxDepthStencil&&) = delete;
	};

}