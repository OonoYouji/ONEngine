#include "DxRenderTexture.h"

#include <d3dx12.h>

#include <DxCommon.h>
#include <DxDescriptor.h>



ComPtr<ID3D12Resource> DxRenderTexture::CreateRenderTextureResource(
	ID3D12Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

	ComPtr<ID3D12Resource> resource;

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

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
	ONE::DxDescriptor* descriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();
	

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearColor_ = clearColor;
	renderTextureResource_ =
		CreateRenderTextureResource(device, 1280, 720, format, clearColor_);


	rtvHandle_.cpuHandle = descriptor->GetRtvCpuHandle();
	descriptor->AddRtvUsedCount();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, rtvHandle_.cpuHandle);


	srvHandle_.cpuHandle = descriptor->GetSrvCpuHandle();
	srvHandle_.gpuHandle = descriptor->GetSrvGpuHandle();
	descriptor->AddSrvUsedCount();

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = format;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(renderTextureResource_.Get(), &renderTextureSrvDesc, srvHandle_.cpuHandle);
}

void DxRenderTexture::SetRenderTarget(
	ID3D12GraphicsCommandList* commandList, ONE::DxDescriptor* dxDescriptor) {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxDescriptor->GetDsvCpuHandle();
	commandList->OMSetRenderTargets(1, &rtvHandle_.cpuHandle, FALSE, &dsvHandle);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->ClearRenderTargetView(rtvHandle_.cpuHandle, &clearColor_.x, 0, nullptr);
}
