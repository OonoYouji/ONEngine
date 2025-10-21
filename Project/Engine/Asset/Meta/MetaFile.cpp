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

	static const uint32_t kCurrentMetaFileVersion = 1;



	bool StartsWith(const std::string& str, const std::string& prefix) {
		return str.rfind(prefix, 0) == 0;
	}


} // namespace




MetaFile::MetaFile() : guid(), assetType(AssetType::Texture) {}
MetaFile::~MetaFile() = default;

bool MetaFile::LoadFromFile(const std::string& _metaFilePath) {
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
		if (StartsWith(line, "version: ")) {
			std::string versionStr = line.substr(9);
			uint32_t version = static_cast<uint32_t>(std::stoul(versionStr));
			if (version != kCurrentMetaFileVersion) {
				/// バージョンが異なる場合はエラー
				Assert(false, "MetaFile version mismatch");
				return false;
			}

		} else if (StartsWith(line, "guid: ")) {
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


	/// 一度ファイルに保存しておく
	metaFile.SaveToFile(_refFile + ".meta");

	return metaFile;
}
