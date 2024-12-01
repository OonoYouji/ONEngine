#pragma once

/// directX
#include <dxcapi.h>
#include <wrl/client.h>

/// std
#include <vector>
#include <string>


class ShaderBlob {
public:

	enum SHADER_TYPE : size_t {
		VS,
		PS,
		GS,
		CS,
		COUNT
	};

	ShaderBlob() = default;
	~ShaderBlob() = default;

	void Compile(const std::wstring& _filePath, const std::wstring& _profile, SHADER_TYPE _shaderType);

	IDxcBlob* GetBlob(SHADER_TYPE _shaderType);

private:

	std::vector<Microsoft::WRL::ComPtr<IDxcBlob>> blobs_;

};