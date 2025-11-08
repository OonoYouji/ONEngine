#include "DxDepthStencil.h"

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

		uint32_t descriptorIndex = _dxDsvHeap->Allocate();

		_dxDevice->GetDevice()->CreateDepthStencilView(
			depthStencilResource_.Get(), &desc, 
			_dxDsvHeap->GetCPUDescriptorHandel(descriptorIndex)
		);
	}

	{	/// srv descriptor
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels = 1;
	
		uint32_t descriptorIndex = _dxSrvHeap->AllocateBuffer();
		
		_dxDevice->GetDevice()->CreateShaderResourceView(
			depthStencilResource_.Get(), &desc,
			_dxSrvHeap->GetCPUDescriptorHandel(descriptorIndex)
		);

		depthSrvHandle_ = descriptorIndex;
	}

	Console::Log("dx depth stencil create success!!");
}
