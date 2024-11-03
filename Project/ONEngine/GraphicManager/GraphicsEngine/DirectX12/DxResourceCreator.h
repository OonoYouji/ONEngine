#pragma once

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace ONE {

	/// ===================================================
	/// Resourceの生成
	/// ===================================================
	class DxResourceCreator {
		DxResourceCreator() = default;
		~DxResourceCreator() = default;
	public:

		/// <summary>
		/// ID3D12Resourceの生成
		/// </summary>
		/// <param name="sizeInByte">resourceのsize</param>
		/// <returns></returns>
		static ComPtr<ID3D12Resource> CreateResource(size_t sizeInByte);

	private:
		DxResourceCreator(const DxResourceCreator&) = delete;
		DxResourceCreator(DxResourceCreator&&) = delete;
		DxResourceCreator& operator= (const DxResourceCreator&) = delete;
		DxResourceCreator& operator= (DxResourceCreator&&) = delete;
	};


}