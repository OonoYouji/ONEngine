#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace ONE {

	class DxDebug final {
	public:

		DxDebug();
		~DxDebug();

		void Initialize(ID3D12Device* device);
		
		void SetDebugLayer();

	private:

#ifdef _DEBUG
		ComPtr<ID3D12Debug1> debugController_ = nullptr;
#endif // _DEBUG

	private:
		DxDebug(const DxDebug&) = delete;
		DxDebug(DxDebug&&) = delete;
		DxDebug& operator=(const DxDebug&) = delete;
		DxDebug& operator=(DxDebug&&) = delete;
	};

}