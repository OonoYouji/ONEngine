#pragma once

/// directx
#include <d3d12.h>

/// std
#include <vector>
#include <string>
#include <optional>

/// engine
#include "Engine/DirectX12/ComPtr/ComPtr.h"
#include "Shader.h"


/// ===================================================
/// グラフィクス用	pipeline
/// ===================================================
class GraphicsPipeline {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GraphicsPipeline();
	~GraphicsPipeline();

	/// @brief 今までにセットした値を使ってパイプラインを生成する
	void CreatePipeline(class DxDevice* _dxDevice);


	/*--- root signature ---*/

	/// @brief 使用するshaderへのポインタをセットする
	/// @param _shader 使用するshader
	void SetShadaer(Shader* _shader);

	/// @brief InputElementを追加する
	/// @param _semanticName   セマンティクスの名前
	/// @param _semanticIndex  セマンティクスのインデックス
	/// @param _format         フォーマットの種類
	void AddInputElement(const std::string& _semanticName, uint32_t _semanticIndex, DXGI_FORMAT _format);

	/// @brief constant buffer viewを追加する
	/// @param _shaderVisibilty shaderの種類
	/// @param _shaderRegister  register(b0)の0の部分
	void AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibilty, uint32_t _shaderRegister);

	/// @brief descriptor rangeを追加する
	/// @param _baseShaderRegister register(b0)の0の部分
	/// @param _numDescriptor      descriptorの数
	/// @param _rangeType          descriptorの種類(CBV, SRV, UAV)
	void AddDescriptorRange(uint32_t _baseShaderRegister, uint32_t _numDescriptor, D3D12_DESCRIPTOR_RANGE_TYPE  _rangeType);

	/// @brief descriptor tableを追加する
	/// @param _shaderVisibilty 使用するshaderの種類(vs, ps)
	/// @param _descriptorIndex descriptor rangeの配列のインデックス
	void AddDescriptorTable(D3D12_SHADER_VISIBILITY _shaderVisibilty, uint32_t _descriptorIndex);

	/// @brief static samplerを追加する
	/// @param _shaderVisibility 使用するshaderの種類(vs, ps)
	/// @param _shaderRegister   shaderのregister(s0)の0の部分
	void AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

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

	/// @brief depth stencil descを設定する
	/// @param _desc 
	void SetDepthStandilDesc(const D3D12_DEPTH_STENCIL_DESC& _desc);

	/// @brief blend descを設定する
	/// @param _desc 設定するblend desc
	void SetBlendDesc(const D3D12_BLEND_DESC& _desc);

	/// @brief render target viewの数を設定する
	/// @param _rtvNum rtvの数
	void SetRTVNum(uint32_t _rtvNum);

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief root signatureを生成する
	void CreateRootSignature(class DxDevice* _dxDevice);

	/// @brief pipeline state objectを生成する
	void CreatePipelineStateObject(class DxDevice* _dxDevice);



private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature>            rootSignature_;
	ComPtr<ID3D12PipelineState>            pipelineState_;


	/// parameters
	std::vector<D3D12_INPUT_ELEMENT_DESC>  inputElements_;
	std::vector<std::string>               semanticNames_;

	D3D12_RASTERIZER_DESC                  rasterizerDesc_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE          primitiveTopologyType_;
	D3D12_BLEND_DESC                       blendDesc_;

	std::vector<D3D12_ROOT_PARAMETER>      rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>    descriptorRanges_;

	Shader*                                pShader_;


	/// pipeline settings
	std::optional<D3D12_DEPTH_STENCIL_DESC> depthStancilDesc_;
	uint32_t                                rtvNum_ = 1;

};
