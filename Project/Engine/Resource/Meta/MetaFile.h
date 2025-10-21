#pragma once

/// engine
#include "Engine/Resource/Guid/Guid.h"


/// ////////////////////////////////////////////////////
/// アセットの種類
/// ////////////////////////////////////////////////////
enum class AssetType {
	Texture,
	Mesh,
	Audio,
};


/// ////////////////////////////////////////////////////
/// .pngなどに付随するメタファイルクラス
/// ////////////////////////////////////////////////////
class MetaFile final {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	MetaFile();
	~MetaFile();

	bool LoadFromFile(const std::string& _metaFilePath);
	bool SaveToFile(const std::string& _metaFilePath) const;

	/// ==================================================
	/// public : objects
	/// ==================================================

	Guid guid;
	AssetType assetType;


};



/// @brief MetaFileを生成する
/// @param _refFile .pngなどの参照ファイルパス
/// @return 生成されたMetaFileオブジェクト
MetaFile GenerateMetaFile(const std::string& _refFile);
