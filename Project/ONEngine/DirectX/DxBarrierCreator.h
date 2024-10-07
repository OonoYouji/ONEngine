#pragma once

#include <d3d12.h>

namespace ONE {

	class DxBarrierCreator final {
		DxBarrierCreator() = default;
		~DxBarrierCreator() = default;
	public:

		static void CreateBarrier(ID3D12Resource* resourec, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	private:
		DxBarrierCreator(const DxBarrierCreator&) = delete;
		DxBarrierCreator(DxBarrierCreator&&) = delete;
		DxBarrierCreator& operator=(const DxBarrierCreator&) = delete;
		DxBarrierCreator& operator=(DxBarrierCreator&&) = delete;
	};

}