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
class AssetLoader<Shader, ShaderMeta> : public IAssetLoader {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	AssetLoader();
	~AssetLoader() override = default;


	std::optional<Shader> Load(const std::string& filepath, Meta<ShaderMeta> meta);
	std::optional<Shader> Reload(const std::string& filepath, Shader* src, Meta<ShaderMeta> meta);
	Meta<ShaderMeta> GetMetaData(const std::string& filepath);


private:

};


} /// namespace ONEngine::Asset