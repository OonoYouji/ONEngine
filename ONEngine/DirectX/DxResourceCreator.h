#pragma once

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace ONE {

	/// ===================================================
	/// Resourceの生成
	/// ===================================================
	class DxResourceCreate {
		DxResourceCreate() = default;
		~DxResourceCreate() = default;
	public:

		/// <summary>
		/// ID3D12Resourceの生成
		/// </summary>
		/// <param name="sizeInByte">resourceのsize</param>
		/// <returns></returns>
		static ComPtr<ID3D12Resource> CreateResource(size_t sizeInByte);

	private:
		DxResourceCreate(const DxResourceCreate&) = delete;
		DxResourceCreate(DxResourceCreate&&) = delete;
		DxResourceCreate& operator= (const DxResourceCreate&) = delete;
		DxResourceCreate& operator= (DxResourceCreate&&) = delete;
	};


}