#pragma once

/// directX
#include <d3d12.h>

/// std
#include <vector>
#include <string>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Shader.h"

/// ///////////////////////////////////////////////////
/// ComputePipeline
/// ///////////////////////////////////////////////////
namespace ONEngine {

class ComputePipeline {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ComputePipeline();
	~ComputePipeline();

	/// @brief pipelineを生成する
	/// @param _dxDevice DxDeviceへのポインタ
	void CreatePipeline(class DxDevice* _dxDevice);


	/*--- root signature ---*/

	/// @brief 使用する shaderへのポインタをセットする
	/// @param _shader 使用するshader
	void SetShader(Shader* _shader);

	/// @brief constant buffer viewを追加する
	/// @param _shaderVisibility shaderの種類
	/// @param _shaderRegister  register(b0)の0の部分
	void AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

	/// @brief 32bit constantを追加する
	/// @param _shaderVisibility shaderの種類
	/// @param _shaderRegister   register(b0)の0の部分
	void Add32BitConstant(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister, uint32_t _num32bitValue = 1u);

	/// @brief descriptor rangeを追加する
	/// @param _baseShaderRegister register(b0)の0の部分
	/// @param _numDescriptor      descriptorの数
	/// @param _rangeType          descriptorの種類(CBV, SRV, UAV)
	void AddDescriptorRange(uint32_t _baseShaderRegister, uint32_t _numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE  _rangeType);

	/// @brief descriptor tableを追加する
	/// @param _shaderVisibility 使用するshaderの種類(vs, ps)
	/// @param _descriptorIndex descriptor rangeの配列のインデックス
	void AddDescriptorTable(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _descriptorIndex);

	/// @brief static samplerを追加する
	/// @param _shaderVisibility 使用するshaderの種類(vs, ps)
	/// @param _shaderRegister   shaderのregister(s0)の0の部分
	/// @param _isComparisonSampler 比較サンプラーにするかどうか
	void AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister, bool _isComparisonSampler = false);

	/// @brief fill modeを設定する
	/// @param _fillMode 設定するfill mode
	void SetFillMode(D3D12_FILL_MODE _fillMode);

	/// @brief カリングの設定
	/// @param _cullMode カリングモード
	void SetCullMode(D3D12_CULL_MODE _cullMode);

	/// @brief TopologyTypeを設定する
	/// @param _topologyType 設定するtopology type
	void SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE _topologyType);


	/*--- pipeline state ---*/

	/// @brief コマンドリストにパイプラインステートをセットする
	/// @param _dxCommand command listを管理しているクラスへのポインタ
	void SetPipelineStateForCommandList(class DxCommand* _dxCommand);


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief root signatureを生成する
	/// @param _dxDevice DxDeviceへのポインタ
	void CreateRootSignature(class DxDevice* _dxDevice);

	/// @brief pipeline state objectを生成する
	/// @param _dxDevice DxDeviceへのポインタ
	void CreatePipelineStateObject(class DxDevice* _dxDevice);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Shader* shader_;

	ComPtr<ID3D12RootSignature>             rootSignature_;
	ComPtr<ID3D12PipelineState>             pipelineState_;

	D3D12_RASTERIZER_DESC                   rasterizerDesc_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE           primitiveTopologyType_;
	D3D12_BLEND_DESC                        blendDesc_;

	std::vector<D3D12_ROOT_PARAMETER>       rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC>  staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>     descriptorRanges_;
};


} /// ONEngine
