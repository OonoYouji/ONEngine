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
	};

public:

	PipelineState();
	~PipelineState();


	/// ===================================================
	/// public : methods
	/// ===================================================


	void CreaeteShader(
		const std::wstring& vsFilePath, const wchar_t* vsProfile,
		const std::wstring& psFilePath, const wchar_t* psProfile
	);

	void Initialize();

	void AddInputElement(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format);

	void SetFillMode(FillMode fillMode);

	void SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

private:

	void CreateRootSignature(ID3D12Device* device);

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

	Shader shader_;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements_;
	std::vector<std::string> semanticNames_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_{};

	//std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
	//std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;

	//std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;

};