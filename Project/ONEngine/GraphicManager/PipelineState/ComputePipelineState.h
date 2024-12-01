#pragma once

/// directX
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <vector>

using namespace Microsoft::WRL;


/// ===================================================
/// CSのPSOクラス
/// ===================================================
class ComputePipelineState {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ComputePipelineState();
	~ComputePipelineState();

	void Create();

	void CreateRootSignature(ID3D12Device* _device);
	
	void CreatePipelineState(ID3D12Device* _device);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	std::vector<D3D12_ROOT_PARAMETER>      rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>    descriptorRanges_;

};