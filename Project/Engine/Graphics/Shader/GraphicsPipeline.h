#pragma once

/// directx
#include <d3d12.h>

/// std
#include <vector>
#include <string>

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
	void CreatePipeline();


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



private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateRootSignature();
	void CreatePipelineStateObject();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipelineState_;

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

};
