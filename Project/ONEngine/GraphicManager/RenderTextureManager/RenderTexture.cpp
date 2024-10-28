#include "RenderTexture.h"

#include <d3dx12.h>

#include <Core/ONEngine.h>

#include "WindowManager/WinApp.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"

#include "GraphicManager/RenderTextureManager/RenderTextureManager.h"

#include "Math/Matrix4x4.h"
#include "Math/Vector2.h"



ComPtr<ID3D12Resource> RenderTexture::CreateRenderTextureResource(
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



void RenderTexture::Initialize(
	const Vector4& clearColor,
	ID3D12GraphicsCommandList* commandList, 
	ONE::DxDescriptorHeap<ONE::HeapType::CBV_SRV_UAV>* _srvDescriptorHeap,
	ONE::DxDescriptorHeap<ONE::HeapType::RTV>* _rtvDescriptorHeap,
	ONE::DxDescriptorHeap<ONE::HeapType::DSV>* _dsvDescriptorHeap) {

	/// setting
	ID3D12Device* device = ONEngine::GetDxCommon()->GetDevice();
	pCommandList_ = commandList;
	pDSVDescriptorHeap_ = _dsvDescriptorHeap;
	currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	/// textureの作成
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearColor_ = clearColor;
	renderTextureResource_ =
		CreateRenderTextureResource(device, 1280, 720, format, clearColor_);


	/// rtvの生成
	uint32_t rtvDescriptorIndex = _rtvDescriptorHeap->Allocate();
	rtvHandle_.cpuHandle = _rtvDescriptorHeap->GetCPUDescriptorHandel(rtvDescriptorIndex);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, rtvHandle_.cpuHandle);



	/// srvの生成
	uint32_t srvDescriptorIndex = _srvDescriptorHeap->Allocate();
	srvHandle_.cpuHandle = _srvDescriptorHeap->GetCPUDescriptorHandel(srvDescriptorIndex);
	srvHandle_.gpuHandle = _srvDescriptorHeap->GetGPUDescriptorHandel(srvDescriptorIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = format;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(renderTextureResource_.Get(), &renderTextureSrvDesc, srvHandle_.cpuHandle);
}

void RenderTexture::SetRenderTarget() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDSVDescriptorHeap_->GetCPUDescriptorHandel(0);
	pCommandList_->OMSetRenderTargets(1, &rtvHandle_.cpuHandle, FALSE, &dsvHandle);
	pCommandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	pCommandList_->ClearRenderTargetView(rtvHandle_.cpuHandle, &clearColor_.x, 0, nullptr);
}

void RenderTexture::BeginRenderTarget() {

	ONE::DxBarrierCreator::CreateBarrier(
		GetRenderTexResource(),
		currentResourceState,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	currentResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	SetRenderTarget();
}

void RenderTexture::EndRenderTarget() {

	ONE::DxBarrierCreator::CreateBarrier(
		GetRenderTexResource(),
		currentResourceState,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

}

void RenderTexture::ClearDepth() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = pDSVDescriptorHeap_->GetCPUDescriptorHandel(0);
	pCommandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}



void RenderTexture::BlendRenderTexture(RenderTexture* frontRenderTex, RenderTexture* output) {

	output->BeginRenderTarget();

	RenderTextureManager::GetInstance()->BindForCommandList();

	/// render tex
	pCommandList_->SetGraphicsRootDescriptorTable(1, this->srvHandle_.gpuHandle);
	pCommandList_->SetGraphicsRootDescriptorTable(2, frontRenderTex->srvHandle_.gpuHandle);

	pCommandList_->DrawInstanced(3, 1, 0, 0);
	output->EndRenderTarget();
}
