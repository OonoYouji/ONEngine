#include "MetaFile.h"

/// std
#include <fstream>
#include <filesystem>

/// engine
#include "Engine/Core/Utility/Utility.h"


namespace {

	std::unordered_map<std::string, AssetType> gExtensionToAssetType = {
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		//{ ".bmp", AssetType::Texture },
		//{ ".tga", AssetType::Texture },
		//{ ".fbx", AssetType::Mesh },
		{ ".obj", AssetType::Mesh },
		{ ".wav", AssetType::Audio },
		{ ".mp3", AssetType::Audio },
		//{ ".ogg", AssetType::Audio },
	};

} // namespace




MetaFile::MetaFile() : guid(), assetType(AssetType::Texture) {}
MetaFile::~MetaFile() = default;

bool MetaFile::LoadFromFile(const std::string& _metaFilePath) {
	bool result = false;

	/// FilePathから.metaを開く


	return result;
}

bool MetaFile::SaveToFile(const std::string& _metaFilePath) const {
	/// --------------------------------------------------
	/// ファイル名の決定 (FileName.meta) になるようにする
	/// --------------------------------------------------

	//std::string filename = _metaFilePath;
	//std::string directory = "";

	///// "/" または "\" の最後の位置を取得して、それ以前を削除
	//size_t lastSlashPos = filename.find_last_of("/\\");
	//if (lastSlashPos != std::string::npos) {
	//	directory = filename.substr(0, lastSlashPos + 1);
	//	filename = filename.substr(lastSlashPos + 1);
	//	filename += ".meta";
	//}


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

	/// Guidの保存
	ofs << "guid: " << guid.ToString() << "\n";


	/// ファイルを閉じる
	ofs.close();

	return true;
}

MetaFile GenerateMetaFile(const std::string& _refFile) {
	MetaFile metaFile;

	/// Guidの生成
	metaFile.guid = GenerateGuid();

	/// 拡張子からアセットタイプを決定
	std::string extension = Mathf::FileExtension(_refFile);
	auto itr = gExtensionToAssetType.find(extension);
	if (itr != gExtensionToAssetType.end()) {
		metaFile.assetType = itr->second;
	} else {
		/// デフォルト値にしとく
		metaFile.assetType = AssetType::Texture; 
	}

	return metaFile;
}
