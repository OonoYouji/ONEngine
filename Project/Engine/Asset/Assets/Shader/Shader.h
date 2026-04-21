#pragma once

/// dx
#include <Windows.h>
#include <dxcapi.h>

/// std
#include <string>

/// engine
#include "../IAsset.h"
#include "../../../Core/DirectX12/ComPtr/ComPtr.h"



namespace ONEngine::Asset {
template<typename T, typename U>
class AssetLoader;
}



namespace ONEngine::Asset {



/// @brief Shaderの種類
enum class ShaderStage {
	Unkown,
	Vertex,
	Pixel,
	Compute,
	Amplification,
	Mesh,
};

void from_json(const nlohmann::json& j, ShaderStage& stage);

void to_json(nlohmann::json& j, const ShaderStage& stage);


/// @brief Shaderのメタデータ
struct ShaderMeta {
	ShaderStage stage;
	std::string entryPoint;
	std::string profile;
};



/// ///////////////////////////////////////////////////
/// HLSLのアセット化
/// ///////////////////////////////////////////////////
class Shader : public IAsset {
	friend class AssetLoader<Shader, ShaderMeta>;
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