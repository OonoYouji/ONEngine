#include "DxDepthStencil.h"

/// directX
#include <d3dx12.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../Device/DxDevice.h"
#include "../DescriptorHeap/DxDSVHeap.h"
#include "../DescriptorHeap/DxSRVHeap.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"

DxDepthStencil::DxDepthStencil() {}
DxDepthStencil::~DxDepthStencil() {}


void DxDepthStencil::Initialize(DxDevice* _dxDevice, DxDSVHeap* _dxDsvHeap, DxSRVHeap* _dxSrvHeap) {
	/// ----- depth stencil 作成 ----- ///
	
	{	/// depth stencil resource
		D3D12_RESOURCE_DESC desc{};
		desc.Width                                = static_cast<UINT64>(EngineConfig::kWindowSize.x);
		desc.Height                               = static_cast<UINT64>(EngineConfig::kWindowSize.y);
		desc.MipLevels                            = 1;
		desc.DepthOrArraySize                     = 1;
		//desc.Format                               = DXGI_FORMAT_D32_FLOAT;
		desc.Format                               = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count                     = 1;
		desc.Dimension                            = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//desc.Flags                                = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		desc.Flags                                = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type                       = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE depthClearValue{};
		depthClearValue.DepthStencil.Depth        = 1.0f;
		depthClearValue.Format                    = DXGI_FORMAT_D32_FLOAT;

		HRESULT hr = _dxDevice->GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&depthStencilResource_)
		);

		Assert(SUCCEEDED(hr), "Failed to create dx depth stencil.");
	}


	{	/// dsv descriptor
		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
		desc.Format              = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension       = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags               = D3D12_DSV_FLAG_NONE;

		depthDsvHandle_ = _dxDsvHeap->Allocate();
		_dxDevice->GetDevice()->CreateDepthStencilView(
			depthStencilResource_.Get(), &desc, 
			_dxDsvHeap->GetCPUDescriptorHandel(depthDsvHandle_)
		);
	}

	{	/// srv descriptor
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels = 1;
	
		depthSrvHandle_ = _dxSrvHeap->AllocateBuffer();
		_dxDevice->GetDevice()->CreateShaderResourceView(
			depthStencilResource_.Get(), &desc,
			_dxSrvHeap->GetCPUDescriptorHandel(depthSrvHandle_)
		);

	}

	currentResourceState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	Console::Log("dx depth stencil create success!!");
}

void DxDepthStencil::CreateBarrierPixelShaderResource(ID3D12GraphicsCommandList* _cmdList) {

	/// すでにpixel shader resourceなら何もしない
	if(currentResourceState_ == D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) {
		Console::LogWarning("DxDepthStencil::CreateBarrierPixelShaderResource(): already in pixel shader resource state.");
		return;
	}


	/// depth stencil -> pixel shader resource
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		depthStencilResource_.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);

	_cmdList->ResourceBarrier(1, &barrier);
	currentResourceState_ = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
}

void DxDepthStencil::CreateBarrierDepthWrite(ID3D12GraphicsCommandList* _cmdList) {

	/// すでにdepth writeなら何もしない
	if(currentResourceState_ == D3D12_RESOURCE_STATE_DEPTH_WRITE) {
		Console::LogWarning("DxDepthStencil::CreateBarrierDepthWrite(): already in depth write state.");
		return;
	}

	/// pixel shader resource -> depth stencil
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		depthStencilResource_.Get(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	_cmdList->ResourceBarrier(1, &barrier);
	currentResourceState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;

}

uint32_t DxDepthStencil::GetDepthSrvHandle() const {
	return depthSrvHandle_;
}

uint32_t DxDepthStencil::GetDepthDsvHandle() const {
	return depthDsvHandle_;
}
