#include "DxRenderTexture.h"

#include <DxCommon.h>
#include <DxDescriptor.h>


ComPtr<ID3D12Resource> DxRenderTexture::CreateRenderTextureResource(
	ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

	ComPtr<ID3D12Resource> resource;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;

	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource)
	);

	return resource;
}



void DxRenderTexture::Initialize(const Vector4& clearColor) {
	ID3D12Device* device = ONE::DxCommon::GetInstance()->GetDevice();

	clearColor_ = clearColor;
	auto renderTextureResource = CreateRenderTextureResource(device, 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	ONE::DxDescriptor* descriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();
	cpuHandle_ = descriptor->GetSrvCpuHandle();
	gpuHandle_ = descriptor->GetSrvGpuHandle();
	descriptor->AddSrvUsedCount();


	device->CreateShaderResourceView(renderTextureResource.Get(), &renderTextureSrvDesc, cpuHandle_);
}
