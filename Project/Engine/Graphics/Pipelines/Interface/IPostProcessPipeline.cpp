#include "IPostProcessPipeline.h"

using namespace ONEngine;

//
#include <d3d12.h>
#include <d3dx12.h>
#include "Engine/Core/Utility/Tools/Log.h"

void ONEngine::CopyResource(ID3D12Resource* _src, ID3D12Resource* _dst, ID3D12GraphicsCommandList6* _cmdList) {
	/// ----- _srcの内容を_dstにコピーする ----- ///

	// 検証用ログ
	static int copyLogCount = 0;
	if (copyLogCount < 10) {
		ONEngine::Console::Log("[Engine] CopyResource: Copying to resource at " + std::to_string((uint64_t)_dst), ONEngine::LogCategory::Engine);
		copyLogCount++;
	}

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
