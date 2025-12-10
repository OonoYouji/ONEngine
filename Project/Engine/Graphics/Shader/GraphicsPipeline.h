#pragma once

/// directx
#include <d3d12.h>

/// std
#include <vector>
#include <string>
#include <optional>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Shader.h"

/// @brief RenderTargetViewのフォーマット
enum class RTVFormat {
	Color         = DXGI_FORMAT_R8G8B8A8_UNORM,     ///< 色
	WorldPosition = DXGI_FORMAT_R16G16B16A16_FLOAT, ///< ワールド座標
	Normal        = DXGI_FORMAT_R16G16B16A16_FLOAT, ///< 法線
	Flags         = DXGI_FORMAT_R32G32B32A32_FLOAT  ///< フラグ
};

/// @brief MRT用RenderTargetViewのインデックス
enum class RTVIndex {
	Color         = 0, ///< 色
	WorldPosition = 1, ///< ワールド座標
	Normal        = 2, ///< 法線
	Flags         = 3, ///< フラグ
	Count /// 要素数
};


/// ///////////////////////////////////////////////////
/// グラフィクス用	pipeline
/// ///////////////////////////////////////////////////
namespace ONEngine {

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

	/// @brief 使用する shaderへのポインタをセットする
	/// @param _shader 使用するshader
	void SetShader(Shader* _shader);

	/// @brief InputElementを追加する
	/// @param _semanticName   セマンティクスの名前
	/// @param _semanticIndex  セマンティクスのインデックス
	/// @param _format         フォーマットの種類
	void AddInputElement(const std::string& _semanticName, uint32_t _semanticIndex, DXGI_FORMAT _format, UINT _inputSlot = 0u);

	/// @brief constant buffer viewを追加する
	/// @param _shaderVisibility shaderの種類
	/// @param _shaderRegister  register(b0)の0の部分
	void AddCBV(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

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
	void AddStaticSampler(D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);
	void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& _samplerDesc, D3D12_SHADER_VISIBILITY _shaderVisibility, uint32_t _shaderRegister);

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

	/// @brief rasterizer descを設定する
	/// @param _desc 
	void SetRasterizerDesc(const D3D12_RASTERIZER_DESC& _desc);

	/// @brief depth stencil descを設定する
	/// @param _desc 
	void SetDepthStencilDesc(const D3D12_DEPTH_STENCIL_DESC& _desc);

	/// @brief blend descを設定する
	/// @param _desc 設定するblend desc
	void SetBlendDesc(const D3D12_BLEND_DESC& _desc);

	/// @brief render target viewの数を設定する
	/// @param _rtvNum rtvの数
	void SetRTVNum(uint32_t _rtvNum);

	/// @brief rtvのフォーマットを設定する
	/// @param _rtvFormats rtvのフォーマットarray
	void SetRTVFormats(const std::vector<DXGI_FORMAT>& _rtvFormats);

	/// @brief rtvのフォーマットを設定する
	/// @param _rtvFormat rtvのフォーマット
	/// @param _rtvIndex setするrtvのインデックス
	void SetRTVFormat(DXGI_FORMAT _rtvFormat, uint32_t _rtvIndex);

	/// @brief コマンドリストにパイプラインステートをセットする
	/// @param _dxCommand command listを管理しているクラスへのポインタ
	void SetPipelineStateForCommandList(class DxCommand* _dxCommand);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief root signatureを生成する
	void CreateRootSignature(class DxDevice* _dxDevice);

	/// @brief pipeline state objectを生成する
	void CreatePipelineStateObject(class DxDevice* _dxDevice);

	void CreateMeshPipelineStateObject(class DxDevice* _dxDevice);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12RootSignature>             rootSignature_;
	ComPtr<ID3D12PipelineState>             pipelineState_;


	/// parameters
	std::vector<D3D12_INPUT_ELEMENT_DESC>   inputElements_;
	std::vector<std::string>                semanticNames_;

	D3D12_RASTERIZER_DESC                   rasterizerDesc_;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE           primitiveTopologyType_;
	D3D12_BLEND_DESC                        blendDesc_;

	std::vector<D3D12_ROOT_PARAMETER>       rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC>  staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE>     descriptorRanges_;

	Shader* pShader_;


	/// pipeline settings
	std::optional<D3D12_DEPTH_STENCIL_DESC> depthStancilDesc_;
	uint32_t                                rtvNum_ = 1;
	std::vector<DXGI_FORMAT>                rtvFormats_;
};

/// @brief DepthStencilDescのデフォルト値を返す
D3D12_DEPTH_STENCIL_DESC DefaultDepthStencilDesc();


/// @brief BlendMode別のBlendDescを返す名前空間
namespace BlendMode {
	D3D12_BLEND_DESC Normal();
	D3D12_BLEND_DESC Add();
	D3D12_BLEND_DESC Subtract();
	D3D12_BLEND_DESC Multiply();
	D3D12_BLEND_DESC Screen();
	D3D12_BLEND_DESC None();
}

namespace StaticSampler {
	D3D12_STATIC_SAMPLER_DESC ClampSampler();
}

} /// ONEngine
