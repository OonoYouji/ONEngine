#include "MaterialLoader.h"

/// std
#include <fstream>

/// engine
#include "Engine/Asset/Meta/MetaFile.h"


namespace ONEngine::Asset {


/**
 * @brief ディスクからマテリアル設定（.mate）ファイルをロードします。
 * @param _filepath ロード対象のファイルパス
 * @param meta マテリアルのメタデータ
 * @return ロードされたMaterialアセット（失敗時はstd::nullopt）
 */
std::optional<Material> AssetLoader<Material>::Load(const std::string& _filepath, Meta<Material::MetaData> meta) {
	/// ファイルを開く
	std::ifstream ifs(_filepath);
	if(!ifs) {
		Console::LogError("[Load Failed] [Material] - File not found: \"" + _filepath + "\"");
		return std::nullopt;
	}


	/// 読み込んだMaterialを格納するオブジェクト
	Material material;
	material.guid = meta.base.guid;

	/// ----------------------------------------------
	/// ファイルの読み込み
	/// ----------------------------------------------
	std::string line;
	while(std::getline(ifs, line)) {
		/// ----- 各文字列ごとに対応した処理を行う ----- ///
		/// guidはmetaファイルから読み込むように変更したが、ファイル内にもある場合はスキップするか上書きするか
		/// ここではMetaファイルを正とする

	}

	/// コンソールにログを出力
	Console::Log("[Load] [Material] - path:\"" + _filepath + "\"");

	return std::move(material);
}

/**
 * @brief 既存のマテリアルに対して再ロード（リロード）を実行します。
 * @param _filepath 再ロード対象のファイルパス
 * @param _src 再ロード元のMaterialオブジェクトへのポインタ
 * @param meta マテリアルのメタデータ
 * @return 再ロードされたMaterialアセット（失敗時はstd::nullopt）
 */
std::optional<Material> AssetLoader<Material>::Reload(const std::string& _filepath, Material* /*_src*/, Meta<Material::MetaData> meta) {
	/// Materialの再読み込みは新規読み込みと同じ処理を行う
	return std::move(Load(_filepath, meta));
}


/**
 * @brief マテリアルアセットに対応するメタデータを取得します。
 * @param _filepath 対象アセットファイルのパス
 * @return 解析・構築されたメタデータオブジェクト
 */
Meta<Material::MetaData> AssetLoader<Material>::GetMetaData(const std::string& _filepath) {
	Meta<Material::MetaData> res{};

	const std::string metaPath = _filepath + ".meta";
	res.base = LoadOrGenerateMetaBase(metaPath, _filepath);

	nlohmann::json j;
	std::ifstream ifs(metaPath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	Material::MetaData data;
	data.useShader = j.value("useShader", std::string(""));
	data.albedoColor = j.value("albedoColor", Vector4::One);
	data.albedoTextureGuid = j.value("albedoTextureGuid", Guid::kInvalid);
	data.normalTextureGuid = j.value("normalTextureGuid", Guid::kInvalid);

	res.data = data;

	return res;
}


} /// namespace ONEngine::Asset