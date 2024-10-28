#pragma once

/// directX12
#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <cstdint>


/// using
using namespace Microsoft::WRL;


namespace ONE {

	/// 前方宣言
	class DxCommon;

	/// ===================================================
	/// Descriptorの種類
	/// ===================================================
	enum class HeapType {
		RTV,
		UAV_CBV_SRV,
		CBV
	};

	/// ===================================================
	/// Descriptorの管理をするtemplateクラス
	/// ===================================================
	template <HeapType type>
	class DescriptorHeap {
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		DescriptorHeap(uint32_t maxHeapSize) : kMaxHeapSize_(maxHeapSize) {}
		~DescriptorHeap() {}

		void Initialize(DxCommon* _dxCommon);

	private:

		ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
		const uint32_t kMaxHeapSize_;
		uint32_t descriptorSize_;

		/// other class pointer
		DxCommon* pDxCommon_ = nullptr;
	};

}