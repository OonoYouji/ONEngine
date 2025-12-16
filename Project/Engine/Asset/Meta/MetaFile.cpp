#include "MetaFile.h"

/// std
#include <fstream>
#include <filesystem>

/// engine
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

namespace {

/// @brief 現在の.metaファイルのバージョン
constexpr uint32_t kCurrentMetaFileVersion = 1;

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
		if (FileSystem::StartsWith(line, "version: ")) {
			/// "version: "の部分を削除して数値部分だけを取得
			const size_t versionStrSize = strlen("version: ");
			std::string versionStr = line.substr(versionStrSize);

			uint32_t version = static_cast<uint32_t>(std::stoul(versionStr));
			if (version != kCurrentMetaFileVersion) {
				/// バージョンが異なる場合はエラー
				Assert(false, "MetaFile version mismatch");
				return false;
			}

		} else if (FileSystem::StartsWith(line, "guid: ")) {
			/// Guidの読み込み
			const size_t guidStrSize = strlen("guid: ");
			std::string guidStr = line.substr(guidStrSize);
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

MetaFile ONEngine::GenerateMetaFile(const std::string& _refFile) {
	/// ----- 新規の.metaファイルを作成する ----- ///

	MetaFile metaFile;

	/// Guidの生成
	metaFile.guid = GenerateGuid();

	/// 拡張子からアセットタイプを決定
	std::string extension = FileSystem::FileExtension(_refFile);
	metaFile.assetType = GetAssetTypeFromExtension(extension);

	/// 一度ファイルに保存しておく
	metaFile.SaveToFile(_refFile + ".meta");

	return metaFile;
}
