#include "ShaderLoader.h"

/// std
#include <fstream>

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

AssetLoader<Shader, ShaderMeta>::AssetLoader() {}

std::optional<Shader> AssetLoader<Shader, ShaderMeta>::Load(const std::string& filepath) {
	/// ----- Shaderの読み込み処理 ----- ///

	MetaFile meta;
	if(!meta.LoadFromFile(filepath + ".meta")) {
		meta = GenerateMetaFile(filepath);
	}

	Shader shader;
	shader.guid = meta.guid;

	/// metaファイルから情報を取得
	if(meta.properties.contains("shaderStage")) {
		auto stage = magic_enum::enum_cast<ShaderStage>(meta.properties.at("shaderStage"));
		if(stage.has_value()) {
			shader.stage_ = stage.value();
		}
	}
	if(meta.properties.contains("entryPoint")) {
		shader.entryPoint_ = meta.properties.at("entryPoint");
	}
	if(meta.properties.contains("profile")) {
		shader.profile_ = meta.properties.at("profile");
	}
	shader.path_ = filepath;

	// ... shader compilation/loading logic ...

	return shader;
}

std::optional<Shader> AssetLoader<Shader, ShaderMeta>::Reload(const std::string& filepath, Shader* src) {
	/// ----- Shaderの再読み込み処理 ----- ///
	return Load(filepath);
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