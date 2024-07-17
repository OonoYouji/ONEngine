#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;


namespace ONE {

	/// ===================================================
	/// DirectX Commandの管理クラス
	/// ===================================================
	class DxCommand final {
	public:
		DxCommand();
		~DxCommand();

		/// ===================================================
		/// public : methods
		/// ===================================================

		void Initialize(ID3D12Device* device);

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================


	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<ID3D12CommandQueue> queue_ = nullptr;
		ComPtr<ID3D12CommandAllocator> allocator_ = nullptr;
		ComPtr<ID3D12GraphicsCommandList> list_ = nullptr;
		

	private:
		DxCommand(const DxCommand&) = delete;
		DxCommand(DxCommand&&) = delete;
		DxCommand& operator=(const DxCommand&) = delete;
		DxCommand& operator=(DxCommand&&) = delete;
	};

}