#include "IPostProcessPipeline.h"

//
#include <d3d12.h>
#include <d3dx12.h>

void CopyResource(ID3D12Resource* _src, ID3D12Resource* _dst, ID3D12GraphicsCommandList6* _cmdList) {

	/// resource barrier
	CD3DX12_RESOURCE_BARRIER uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_src,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	CD3DX12_RESOURCE_BARRIER sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_dst,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	_cmdList->ResourceBarrier(1, &uavTexBarrier);
	_cmdList->ResourceBarrier(1, &sceneTexBarrier);


	/// copy
	_cmdList->CopyResource(_dst, _src);


	/// resource barrier
	uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_src,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_dst,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);


	_cmdList->ResourceBarrier(1, &uavTexBarrier);
	_cmdList->ResourceBarrier(1, &sceneTexBarrier);
}
