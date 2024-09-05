#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>

#include <Vector4.h>

using namespace Microsoft::WRL;

namespace ONE {
	class DxDescriptor;
}

class DxRenderTexture {
public:

	DxRenderTexture() {}
	~DxRenderTexture() {}

	static ComPtr<ID3D12Resource> CreateRenderTextureResource(
		ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor
	);


	void Initialize(const Vector4& clearColor);

	void SetRenderTarget(
		ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor
	);

	ID3D12Resource* GetRenderTexResource() const {
		return renderTextureResource_.Get();
	}

private:

	struct DescriptorHandle final {
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	};


	Vec4 clearColor_;
	DescriptorHandle rtvHandle_;
	DescriptorHandle srvHandle_;

	ComPtr<ID3D12Resource> renderTextureResource_;

};