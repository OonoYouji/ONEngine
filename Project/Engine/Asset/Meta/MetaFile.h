#pragma once

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/AssetType.h"

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

	/// @brief ファイルの読み込み
	/// @param _metaFilePath .mataファイルのパス
	/// @return true: 読み込み成功, false: 読み込み失敗
	bool LoadFromFile(const std::string& _metaFilePath);

	/// @brief ファイルの保存
	/// @param _metaFilePath 保存先の.metaファイルパス
	/// @return true: 保存成功, false: 保存失敗
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
