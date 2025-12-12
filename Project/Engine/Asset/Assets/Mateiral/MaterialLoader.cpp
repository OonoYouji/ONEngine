#include "MaterialLoader.h"

/// std
#include <fstream>

using namespace ONEngine;

Material AssetLoaderT<Material>::Load(const std::string& _filepath) {
	/// ファイルを開く
	std::ifstream ifs(_filepath);
	if (!ifs) {
		Console::LogError("[Load Failed] [Material] - File not found: \"" + _filepath + "\"");
		return {};
	}


	/// 読み込んだMaterialを格納するオブジェクト
	Material material;

	/// ----------------------------------------------
	/// ファイルの読み込み
	/// ----------------------------------------------
	std::string line;
	while (std::getline(ifs, line)) {
		/// ----- 各文字列ごとに対応した処理を行う ----- ///
		if (FileSystem::StartsWith(line, "guid: ")) {
			/// ファイルの文字列をGuidに変換して格納
			std::string guidStr = line.substr(6);
			material.guid = Guid::FromString(guidStr);
		}

	}

	/// コンソールにログを出力
	Console::Log("[Load] [Material] - path:\"" + _filepath + "\"");

	return std::move(material);
}

Material AssetLoaderT<Material>::Reload(const std::string& _filepath, Material* /*_src*/) {
	/// Materialの再読み込みは新規読み込みと同じ処理を行う
	return std::move(Load(_filepath));
}

