#include <DxResourceCreator.h>

#include <cassert>

#include <DxDevice.h>
#include <DxCommon.h>


/// ===================================================
/// Resourceの生成
/// ===================================================
ComPtr<ID3D12Resource> ONE::DxResourceCreator::CreateResource(size_t sizeInByte) {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12Resource> resource = nullptr;

	///- 頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	///- 頂点リソースの設定
	D3D12_RESOURCE_DESC desc{};
	///- バッファリソース
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = sizeInByte; //- リソースのサイズ

	///- バッファの場合これらは1に設定する決まり
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	///- バッファの場合はこれにする決まり
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	///- 実際に頂点リソースを生成
	result = DxCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));

	return resource;
}
