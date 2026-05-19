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
template<typename T>
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



/// ///////////////////////////////////////////////////
/// HLSLのアセット化
/// ///////////////////////////////////////////////////
class Shader : public IAsset {
	friend class AssetLoader<Shader>;
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	/// @brief Shaderのメタデータ
	struct MetaData {
		ShaderStage stage;
		std::string entryPoint;
		std::string profile;
	};


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