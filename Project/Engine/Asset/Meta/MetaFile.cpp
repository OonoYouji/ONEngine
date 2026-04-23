#include "MetaFile.h"

/// std
#include <fstream>
#include <filesystem>

/// externals
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Core/Utility/Utility.h"

namespace {

/// @brief 現在の.metaファイルのバージョン
//constexpr uint32_t kCurrentMetaFileVersion = 1;

} // namespace


namespace ONEngine::Asset {
//
//
//MetaFile::MetaFile() : guid(), assetType(AssetType::None) {}
//MetaFile::~MetaFile() = default;
//
//bool MetaFile::LoadFromFile(const std::string& _metaFilePath) {
//	/// ----- .metaファイルを読み込む ----- ///
//
//	/// FilePathから.metaを開く
//	std::ifstream ifs(_metaFilePath);
//	if(!ifs) {
//		return false;
//	}
//
//	/// ファイルの読み込み
//	std::string line;
//	while(std::getline(ifs, line)) {
//		/// バージョンの読み込み
//		if(FileSystem::StartsWith(line, "version: ")) {
//			/// "version: "の部分を削除して数値部分だけを取得
//			const size_t versionStrSize = strlen("version: ");
//			std::string versionStr = line.substr(versionStrSize);
//
//			uint32_t version = static_cast<uint32_t>(std::stoul(versionStr));
//			if(version != kCurrentMetaFileVersion) {
//				/// バージョンが異なる場合は警告
//				Console::LogWarning("MetaFile version mismatch in: " + _metaFilePath);
//			}
//
//		} else if(FileSystem::StartsWith(line, "guid: ")) {
//			/// Guidの読み込み
//			const size_t guidStrSize = strlen("guid: ");
//			std::string guidStr = line.substr(guidStrSize);
//			guid = Guid::FromString(guidStr);
//		} else if(FileSystem::StartsWith(line, "type: ")) {
//			/// AssetTypeの読み込み
//			const size_t typeStrSize = strlen("type: ");
//			std::string typeStr = line.substr(typeStrSize);
//			auto type = magic_enum::enum_cast<AssetType>(typeStr);
//			if(type.has_value()) {
//				assetType = type.value();
//			}
//		} else {
//			/// それ以外はプロパティとして読み込む (key: value形式)
//			size_t colonPos = line.find(": ");
//			if(colonPos != std::string::npos) {
//				std::string key = line.substr(0, colonPos);
//				std::string value = line.substr(colonPos + 2);
//				properties[key] = value;
//			}
//		}
//	}
//
//
//	return true;
//}
//
//bool MetaFile::SaveToFile(const std::string& _metaFilePath) const {
//	/// --------------------------------------------------
//	/// ファイルの生成
//	/// --------------------------------------------------
//
//	std::ofstream ofs(_metaFilePath);
//
//	/// ファイルが見つからない場合は生成する
//	if(std::filesystem::exists(_metaFilePath) == false) {
//		std::ofstream createOfs(_metaFilePath);
//		createOfs.close();
//	}
//
//
//	/// --------------------------------------------------
//	/// 値の保存
//	/// --------------------------------------------------
//
//	/// file version
//	ofs << "version: " << kCurrentMetaFileVersion << "\n";
//
//	/// Guidの保存
//	ofs << "guid: " << guid.ToString() << "\n";
//
//	/// AssetTypeの保存
//	ofs << "type: " << magic_enum::enum_name(assetType) << "\n";
//
//	/// プロパティの保存
//	for(const auto& [key, value] : properties) {
//		ofs << key << ": " << value << "\n";
//	}
//
//
//	/// ファイルを閉じる
//	ofs.close();
//
//	return true;
//}
//
//MetaFile GenerateMetaFile(const std::string& _refFile) {
//	/// ----- 新規の.metaファイルを作成する ----- ///
//
//	MetaFile metaFile;
//
//	/// Guidの生成
//	metaFile.guid = GenerateGuid();
//
//	/// 拡張子からアセットタイプを決定
//	std::string extension = FileSystem::FileExtension(_refFile);
//	metaFile.assetType = GetAssetTypeFromExtension(extension);
//
//	/// デフォルトのプロパティを設定
//	if(metaFile.assetType == AssetType::Shader) {
//		metaFile.properties["shaderStage"] = "Vertex";
//		metaFile.properties["entryPoint"] = "main";
//		metaFile.properties["profile"] = "vs_6_0";
//	} else if(metaFile.assetType == AssetType::Texture) {
//		metaFile.properties["format"] = "R8G8B8A8_UNORM";
//	}
//
//	/// 一度ファイルに保存しておく
//	metaFile.SaveToFile(_refFile + ".meta");
//
//	return metaFile;
//}

MetaBase LoadMetaBaseFromFile(const std::string& filepath) {

	nlohmann::json j;
	std::ifstream ifs(filepath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	ifs.close();
	MetaBase metaBase;
	metaBase.guid = j.value("guid", Guid{});
	metaBase.type = j.value("type", AssetType{});
	metaBase.name = j.value("name", std::string{});

	return metaBase;
}

} /// namespace ONEngine::Asset