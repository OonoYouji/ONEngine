#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxcapi.h>

#include <vector>
#include <string>

using namespace Microsoft::WRL;


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


	void InitializeShader(
		const std::wstring& vsFilePath, const wchar_t* vsProfile,
		const std::wstring& psFilePath, const wchar_t* psProfile
	);

	void Initialize();


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

	//std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
	//std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;

	//std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges_;

};