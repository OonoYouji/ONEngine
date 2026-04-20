#pragma once

/// dx
#include <Windows.h>
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "../IAsset.h"
#include "../../../Core/DirectX12/ComPtr/ComPtr.h"

namespace ONEngine {


/// ///////////////////////////////////////////////////
/// HLSLのステージを表す列挙型
/// ///////////////////////////////////////////////////
enum class ShaderStage {
	Vertex,
	Pixel,
	Compute,
	Amplification,
	Mesh,
};


/// ///////////////////////////////////////////////////
/// HLSLのアセット化
/// ///////////////////////////////////////////////////
class Shader : public IAsset {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Shader();
	~Shader();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string path_;
	ShaderStage stage_;
	std::string entryPoint_;
	std::string profile_;

	ComPtr<IDxcBlob> shader_;
};

} /// namespace ONEngine