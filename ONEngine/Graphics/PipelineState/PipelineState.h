#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxcapi.h>

#include <vector>
#include <string>

using namespace Microsoft::WRL;


/// ===================================================
/// enum : モデルの表示形式
/// ===================================================
enum FillMode : uint8_t {
	kWireFrame,	//- ワイヤーフレーム
	kSolid,		//- 塗りつぶし

	kCount
};



/// ===================================================
/// pipeline state objcet
/// ===================================================
class PipelineState final {
public:

	struct Shader final {
		ComPtr<IDxcBlob> vs = nullptr;
		ComPtr<IDxcBlob> ps = nullptr;
		ComPtr<IDxcBlob> gs = nullptr;

		/// <summary>
		/// shaderのコンパイル
		/// </summary>
		/// <param name="vsFilePath"> : vertex shader の file path</param>
		/// <param name="vsProfile">  : vertex shader の version</param>
		/// <param name="psFilePath"> : pixel shader  の file path</param>
		/// <param name="psProfile">  : pixel shader  の version</param>
		void ShaderCompile(
			const std::wstring& vsFilePath, const wchar_t* vsProfile,
			const std::wstring& psFilePath, const wchar_t* psProfile
		);

		void GeometryShaderCompile(
			const std::wstring& filePath, const wchar_t* profile
		);

	};

public:

	PipelineState();
	~PipelineState();


	/// ===================================================
	/// public : methods
	/// ===================================================



	/// <summary>
	/// root signature と pipeline stateの初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// shaderのセット
	/// </summary>
	/// <param name="shader"></param>
	void SetShader(Shader* shader);




	/// <summary>
	/// input layout elementの追加
	/// </summary>
	void AddInputElement(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format);
	void AddInputElement(const D3D12_INPUT_ELEMENT_DESC& inputElement);

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

	


	/// <summary>
	/// FillModeのセット
	/// </summary>
	void SetFillMode(FillMode fillMode);

	/// <summary>
	/// 描画する形状のセット
	/// </summary>
	void SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

	/// <summary>
	/// root signatureとpipeline stateをcommand listに設定
	/// </summary>
	void SetPipelineState();

private:

	/// ===================================================
	/// private : methods
	/// ===================================================


	/// <summary>
	/// root signature の生成
	/// </summary>
	/// <param name="device"></param>
	void CreateRootSignature(ID3D12Device* device);

	/// <summary>
	/// pipeline state の生成
	/// </summary>
	/// <param name="device"></param>
	void CreatePipelineState(ID3D12Device* device);

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static const std::wstring kDirectoryPath_;



	/// ===================================================
	/// private : objects
	/// ===================================================

	/*
		- root signature
			- root parameter
			- static sampler
		- pipeline state
			- depth stencil
			- input element
			- blend state
			- raster rizer
			- vs
			- ps
	*/

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	Shader* shader_;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements_;
	std::vector<std::string> semanticNames_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_{};

	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;

};