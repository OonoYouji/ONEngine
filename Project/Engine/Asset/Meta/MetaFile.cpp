#include "MetaFile.h"

/// std
#include <fstream>
#include <filesystem>

/// engine
#include "Engine/Core/Utility/Utility.h"


namespace {

	/// @brief 現在の.metaファイルのバージョン
	static const uint32_t kCurrentMetaFileVersion = 1;

} // namespace




MetaFile::MetaFile() : guid(), assetType(AssetType::Texture) {}
MetaFile::~MetaFile() = default;

bool MetaFile::LoadFromFile(const std::string& _metaFilePath) {
	/// ----- .metaファイルを読み込む ----- ///

	bool result = false;

	/// FilePathから.metaを開く
	std::ifstream ifs(_metaFilePath);
	if (!ifs) {
		return false;
	}

	/// ファイルの読み込み
	std::string line;
	while (std::getline(ifs, line)) {
		/// バージョンの読み込み
		if (Mathf::StartsWith(line, "version: ")) {
			std::string versionStr = line.substr(9);
			uint32_t version = static_cast<uint32_t>(std::stoul(versionStr));
			if (version != kCurrentMetaFileVersion) {
				/// バージョンが異なる場合はエラー
				Assert(false, "MetaFile version mismatch");
				return false;
			}

		} else if (Mathf::StartsWith(line, "guid: ")) {
			/// Guidの読み込み

			std::string guidStr = line.substr(6);
			guid = Guid::FromString(guidStr);
		}
	}


	return result;
}

bool MetaFile::SaveToFile(const std::string& _metaFilePath) const {
	/// --------------------------------------------------
	/// ファイルの生成
	/// --------------------------------------------------

	std::ofstream ofs(_metaFilePath);

	/// ファイルが見つからない場合は生成する
	if (std::filesystem::exists(_metaFilePath) == false) {
		std::ofstream createOfs(_metaFilePath);
		createOfs.close();
	}


	/// --------------------------------------------------
	/// 値の保存
	/// --------------------------------------------------

	/// file version
	ofs << "version: " << kCurrentMetaFileVersion << "\n";

	/// Guidの保存
	ofs << "guid: " << guid.ToString() << "\n";


	/// ファイルを閉じる
	ofs.close();

	return true;
}

MetaFile GenerateMetaFile(const std::string& _refFile) {
	/// ----- 新規の.metaファイルを作成する ----- ///

	MetaFile metaFile;

	/// Guidの生成
	metaFile.guid = GenerateGuid();

	/// 拡張子からアセットタイプを決定
	std::string extension = Mathf::FileExtension(_refFile);
	metaFile.assetType = GetAssetTypeFromExtension(extension);

	/// 一度ファイルに保存しておく
	metaFile.SaveToFile(_refFile + ".meta");

	return metaFile;
}
