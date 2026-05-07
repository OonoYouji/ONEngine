#include "ShaderLoader.h"

/// std
#include <fstream>

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

AssetLoader<Shader>::AssetLoader() {}

std::optional<Shader> AssetLoader<Shader>::Load(const std::string& filepath, Meta<Shader::MetaData> meta) {
	/// ----- Shaderの読み込み処理 ----- ///

	Shader shader;
	shader.guid = meta.base.guid;
	shader.path_ = filepath;

	// ... shader compilation/loading logic ...

	return shader;
}

std::optional<Shader> AssetLoader<Shader>::Reload(const std::string& filepath, Shader* src, Meta<Shader::MetaData> meta) {
	/// ----- Shaderの再読み込み処理 ----- ///
	return Load(filepath, meta);
}

Meta<Shader::MetaData> AssetLoader<Shader>::GetMetaData(const std::string& filepath) {
	Meta<Shader::MetaData> res{};

	res.base = LoadMetaBaseFromFile(filepath);

	nlohmann::json j;
	std::ifstream ifs(filepath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	Shader::MetaData data{};
	data.entryPoint = j.value("entryPoint", "");
	data.profile = j.value("profile", "");
	data.stage = j.value("shaderStage", ShaderStage::Unkown);
	res.data = data;

	return res;
}


} /// namespace ONEngine