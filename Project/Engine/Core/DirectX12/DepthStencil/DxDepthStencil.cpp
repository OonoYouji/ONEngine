#include "DxDepthStencil.h"

/// engine
#include "../Device/DxDevice.h"
#include "../DescriptorHeap/DxDSVHeap.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Core/Utility/DebugTools/Log.h"

DxDepthStencil::DxDepthStencil() {}
DxDepthStencil::~DxDepthStencil() {}


void DxDepthStencil::Initialize(DxDevice* _dxDevice, DxDSVHeap* _dxDsvHeap) {
	
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Width                                = 1280;
		desc.Height                               = 720;
		desc.MipLevels                            = 1;
		desc.DepthOrArraySize                     = 1;
		desc.Format                               = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count                     = 1;
		desc.Dimension                            = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
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


	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
		desc.Format                                         = DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension                                  = D3D12_DSV_DIMENSION_TEXTURE2D;

		uint32_t descriptorIndex                            = _dxDsvHeap->Allocate();

		_dxDevice->GetDevice()->CreateDepthStencilView(
			depthStencilResource_.Get(), &desc, 
			_dxDsvHeap->GetCPUDescriptorHandel(descriptorIndex)
		);
		
	}

	Log("dx depth stencil create success!!");
}
