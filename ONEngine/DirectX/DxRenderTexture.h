#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>

#include <Vector4.h>

using namespace Microsoft::WRL;



class DxRenderTexture {
public:

	static ComPtr<ID3D12Resource> CreateRenderTextureResource(
		ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor
	);


	void Initialize(const Vector4& clearColor);

private:

	Vec4 clearColor_;

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;

};