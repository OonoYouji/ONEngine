#pragma once

/// directX
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <vector>

/// engine
#include "ShaderBlob.h"

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

	void SetShaderBlob(ShaderBlob* _shaderBlob);


	/// <summary>
	/// 定数バッファのパラメータを追加
	/// </summary>
	/// <param name="shaderVisibilty"> : 使用するshader</param>
	/// <param name="shaderRegister"> : レジスタ番号</param>
	void AddCBV(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t shaderRegister);

	/// <summary>
	/// ディスクリプターレンジの追加
	/// </summary>
	/// <param name="baseShaderRegister"> : レジスタ番号</param>
	/// <param name="numDescriptor"> : 使用するSRVの数</param>
	/// <param name="rangeType"></param>
	void AddDescriptorRange(uint32_t baseShaderRegister, uint32_t numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

	/// <summary>
	/// ディスクリプターテーブルの追加
	/// </summary>
	/// <param name="shaderVisibilty"> : 使用するshader</param>
	/// <param name="descriptorIndex"> : DescriptorRangeの配列index !!!範囲外参照に注意!!! </param>
	void AddDescriptorTable(D3D12_SHADER_VISIBILITY shaderVisibilty, uint32_t descriptorIndex);

	/// <summary>
	/// サンプラーの追加
	/// </summary>
	/// <param name="shaderVisibility"></param>
	/// <param name="shaderRegister"></param>
	void AddStaticSampler(D3D12_SHADER_VISIBILITY shaderVisibility, uint32_t shaderRegister);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	std::vector<D3D12_ROOT_PARAMETER>      rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>    descriptorRanges_;

	ShaderBlob* shaderBlob_;

};