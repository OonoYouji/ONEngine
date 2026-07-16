#include "ShaderLoader.h"

/// std
#include <fstream>

/// externals
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

/**
 * @brief コンストラクタ
 */
AssetLoader<Shader>::AssetLoader() {}

/**
 * @brief HLSLファイルをコンパイルし、Shaderオブジェクトとしてロードします。
 * @param filepath ロード対象のファイルパス
 * @param meta シェーダーのメタデータ
 * @return ロードされたShaderアセット（失敗時はstd::nullopt）
 */
std::optional<Shader> AssetLoader<Shader>::Load(const std::string& filepath, Meta<Shader::MetaData> meta) {
	/// ----- Shaderの読み込み処理 ----- ///

	Shader shader;
	shader.guid = meta.base.guid;
	shader.path_ = filepath;

	// ... shader compilation/loading logic ...

	return shader;
}

/**
 * @brief 既存のShaderに対して再ロード（再ビルド）を実行します。
 * @param filepath 再ロード対象のファイルパス
 * @param src 再ロード元のShaderオブジェクトへのポインタ
 * @param meta シェーダーのメタデータ
 * @return 再ロードされたShaderアセット（失敗時はstd::nullopt）
 */
std::optional<Shader> AssetLoader<Shader>::Reload(const std::string& filepath, Shader* /*src*/, Meta<Shader::MetaData> meta) {
	/// ----- Shaderの再読み込み処理 ----- ///
	return Load(filepath, meta);
}

/**
 * @brief シェーダーアセットに対応するメタデータを取得します。
 * @param _filepath 対象アセットファイルのパス
 * @return 解析・構築されたメタデータオブジェクト
 */
Meta<Shader::MetaData> AssetLoader<Shader>::GetMetaData(const std::string& _filepath) {
	Meta<Shader::MetaData> res{};

	const std::string metaPath = _filepath + ".meta";
	res.base = LoadOrGenerateMetaBase(metaPath, _filepath);

	nlohmann::json j;
	std::ifstream ifs(metaPath);
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