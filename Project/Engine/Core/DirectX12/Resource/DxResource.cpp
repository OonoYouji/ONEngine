#include "DxResource.h"

/// engine
#include "../Device/DxDevice.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"


DxResource::DxResource() {}
DxResource::~DxResource() {}

void DxResource::CreateResource(DxDevice* _dxDevice, size_t _sizeInByte) {
	HRESULT result = S_FALSE;

	/// ヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER; /// バッファリソース
	desc.Width            = _sizeInByte;                     /// リソースのサイズ
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	/// リソースの作成
	result = _dxDevice->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);

	Assert(SUCCEEDED(result), "Resource creation failed.");
}
