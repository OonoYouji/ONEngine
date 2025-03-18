#include "RenderTexture.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

void RenderTexture::Initialize(const Vector4& _clearColor, DxManager* _dxManager) {
	clearColor_ = _clearColor;

	
	/// create instance
	//texture_ = std::make_unique<Texture>();

	DxDevice* dxDevice = _dxManager->GetDxDevice();
	DxSRVHeap* dxSRVHeap = _dxManager->GetDxSRVHeap();
	DxRTVHeap* dxRTVHeap = _dxManager->GetDxRTVHeap();
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/// render texture resourceの作成
	renderTextureResource_.CreateRenderTextureResource(
		dxDevice, Vector2(1280.0f, 720.0f), format, _clearColor
	);

	uint32_t rtvHeapIndex = dxRTVHeap->Allocate();
	rtvHandle_.cpuHandle = dxRTVHeap->GetCPUDescriptorHandel(rtvHeapIndex);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	dxDevice->GetDevice()->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, rtvHandle_.cpuHandle);


	/// shader resource viewの作成
	uint32_t srvHeapIndex = dxSRVHeap->AllocateTexture();
	srvHandle_.cpuHandle = dxSRVHeap->GetCPUDescriptorHandel(srvHeapIndex);
	srvHandle_.gpuHandle = dxSRVHeap->GetGPUDescriptorHandel(srvHeapIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	dxDevice->GetDevice()->CreateShaderResourceView(renderTextureResource_.Get(), &srvDesc, srvHandle_.cpuHandle);


	renderTextureResource_.CreateBarrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxManager->GetDxCommand()
	);
}

void RenderTexture::SetRenderTarget(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap) {
	auto command = _dxCommand->GetCommandList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = _dxDSVHeap->GetCPUDescriptorHandel(0);

	command->OMSetRenderTargets(1, &rtvHandle_.cpuHandle, FALSE, &dsvHandle);
	command->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	command->ClearRenderTargetView(rtvHandle_.cpuHandle, &clearColor_.x, 0, nullptr);
}

void RenderTexture::Begin(DxCommand* _dxCommand, DxDSVHeap* _dxDSVHeap) {
	renderTextureResource_.CreateBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		_dxCommand
	);

	SetRenderTarget(_dxCommand, _dxDSVHeap);
}

void RenderTexture::End(DxCommand* _dxCommand) {
	renderTextureResource_.CreateBarrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxCommand
	);
}
