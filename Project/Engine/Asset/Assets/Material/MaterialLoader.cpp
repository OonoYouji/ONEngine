#include "MaterialLoader.h"

/// std
#include <fstream>

/// engine
#include "Engine/Asset/Meta/MetaFile.h"


namespace ONEngine::Asset {


std::optional<Material> AssetLoader<Material, MaterialMeta>::Load(const std::string& _filepath) {
	/// Metaファイルを読み込む
	MetaFile meta;
	if(!meta.LoadFromFile(_filepath + ".meta")) {
		meta = GenerateMetaFile(_filepath);
	}

	/// ファイルを開く
	std::ifstream ifs(_filepath);
	if(!ifs) {
		Console::LogError("[Load Failed] [Material] - File not found: \"" + _filepath + "\"");
		return std::nullopt;
	}


	/// 読み込んだMaterialを格納するオブジェクト
	Material material;
	material.guid = meta.guid;

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

std::optional<Material> AssetLoader<Material, MaterialMeta>::Reload(const std::string& _filepath, Material* /*_src*/) {
	/// Materialの再読み込みは新規読み込みと同じ処理を行う
	return std::move(Load(_filepath));
}


Meta<MaterialMeta> AssetLoader<Material, MaterialMeta>::GetMetaData(const std::string& _filepath) {
	Meta<MaterialMeta> res{};

	res.base = LoadMetaBaseFromFile(_filepath);

	nlohmann::json j;
	std::ifstream ifs(_filepath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	MaterialMeta data;
	data.useShader = j.value("useShader", std::string(""));
	data.albedoColor = j.value("albedoColor", Vector4::One);
	data.albedoTextureGuid = j.value("albedoTextureGuid", Guid::kInvalid);
	data.normalTextureGuid = j.value("normalTextureGuid", Guid::kInvalid);

	res.data = data;

	return res;
}


} /// namespace ONEngine::Asset