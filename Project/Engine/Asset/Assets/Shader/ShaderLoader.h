#pragma once

/// engine
#include "../IAssetLoader.h"
#include "Shader.h"
#include "Engine/Asset/Meta/MetaFile.h"

namespace ONEngine::Asset {


/// /////////////////////////////////////////////////
/// Shader用のアセットローダー
/// /////////////////////////////////////////////////
template<>
class AssetLoader<Shader> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoader();
	~AssetLoader() override = default;


	std::optional<Shader> Load(const std::string& filepath, typename Meta<Shader::MetaData> meta);
	std::optional<Shader> Reload(const std::string& filepath, Shader* src, typename Meta<Shader::MetaData> meta);
	Meta<typename Shader::MetaData> GetMetaData(const std::string& filepath);


private:

};


} /// namespace ONEngine::Asset