#include <DxDepthStencil.h>

#include <WinApp.h>
#include <cassert>


ONE::DxDepthStencil::DxDepthStencil() {}
ONE::DxDepthStencil::~DxDepthStencil() {}

void ONE::DxDepthStencil::Initialize(ID3D12Device* device) {
	HRESULT hr = S_FALSE;

	D3D12_RESOURCE_DESC desc{};
	desc.Width = ONE::WinApp::kWindowSizeX;
	desc.Height = ONE::WinApp::kWindowSizeY;
	desc.MipLevels = 1;
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthStencilResource_)
	);

	assert(SUCCEEDED(hr));

}
