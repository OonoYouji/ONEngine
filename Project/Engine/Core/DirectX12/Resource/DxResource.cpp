#include "DxResource.h"

/// engine
#include "../Device/DxDevice.h"
#include "../Command/DxCommand.h"
#include "Engine/Core/Utility/DebugTools/Assert.h"


DxResource::DxResource() {}
DxResource::~DxResource() {}

void DxResource::CreateResource(DxDevice* _dxDevice, size_t _sizeInByte) {
	HRESULT result = S_FALSE;

	/// ヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; /// バッファリソース
	desc.Width = _sizeInByte;                     /// リソースのサイズ
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

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

void DxResource::CreateCommittedResource(DxDevice* _dxDevice, const D3D12_HEAP_PROPERTIES* _pHeapProperties, D3D12_HEAP_FLAGS _HeapFlags, const D3D12_RESOURCE_DESC* _pDesc, D3D12_RESOURCE_STATES _InitialResourceState, const D3D12_CLEAR_VALUE* _pOptimizedClearValue) {
	_dxDevice->GetDevice()->CreateCommittedResource(
		_pHeapProperties,
		_HeapFlags,
		_pDesc,
		_InitialResourceState,
		_pOptimizedClearValue,
		IID_PPV_ARGS(&resource_)
	);
}

void DxResource::CreateRenderTextureResource(DxDevice* _dxDevice, const Vector2& _size, DXGI_FORMAT _format, const Vector4& _clearColor) {
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		_format,
		static_cast<UINT64>(_size.x),
		static_cast<UINT64>(_size.y),
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = _format;
	clearValue.Color[0] = _clearColor.x;
	clearValue.Color[1] = _clearColor.y;
	clearValue.Color[2] = _clearColor.z;
	clearValue.Color[3] = _clearColor.w;

	_dxDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource_)
	);
}

void DxResource::CreateBarrier(D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	if (_before == _after) {
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource_.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = _before;
	barrier.Transition.StateAfter = _after;

	_dxCommand->GetCommandList()->ResourceBarrier(1, &barrier);
}


void CreateBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	if (_before == _after) { return; }

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = _resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = _before;
	barrier.Transition.StateAfter = _after;

	_dxCommand->GetCommandList()->ResourceBarrier(1, &barrier);
}
