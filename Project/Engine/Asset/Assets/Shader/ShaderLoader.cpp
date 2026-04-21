#include "ShaderLoader.h"

/// std
#include <fstream>

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

AssetLoader<Shader, ShaderMeta>::AssetLoader() {}

std::optional<Shader> AssetLoader<Shader, ShaderMeta>::Load(const std::string& filepath, Meta<ShaderMeta> meta) {
	/// ----- Shaderの読み込み処理 ----- ///

	Shader shader;
	//shader.guid = meta.guid;
	shader.path_ = filepath;

	// ... shader compilation/loading logic ...

	return shader;
}

std::optional<Shader> AssetLoader<Shader, ShaderMeta>::Reload(const std::string& filepath, Shader* src, Meta<ShaderMeta> meta) {
	/// ----- Shaderの再読み込み処理 ----- ///
	return Load(filepath, meta);
}

Meta<ShaderMeta> AssetLoader<Shader, ShaderMeta>::GetMetaData(const std::string& filepath) {
	Meta<ShaderMeta> res{};

	res.base = LoadMetaBaseFromFile(filepath);

	nlohmann::json j;
	std::ifstream ifs(filepath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	ShaderMeta data{};
	data.entryPoint = j.value("entryPoint", "");
	data.profile = j.value("profile", "");
	data.stage = j.value("shaderStage", ShaderStage::Unkown);
	res.data = data;

	return res;
}


} /// namespace ONEngine