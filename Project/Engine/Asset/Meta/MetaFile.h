#pragma once

/// std
#include <unordered_map>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Asset/AssetType.h"

namespace ONEngine::Asset {

///// ////////////////////////////////////////////////////
///// .pngなどに付随するメタファイルクラス
///// ////////////////////////////////////////////////////
//class MetaFile final {
//public:
//	/// ==================================================
//	/// public : methods
//	/// ==================================================
//
//	MetaFile();
//	~MetaFile();
//
//	/// @brief ファイルの読み込み
//	/// @param _metaFilePath .mataファイルのパス
//	/// @return true: 読み込み成功, false: 読み込み失敗
//	bool LoadFromFile(const std::string& _metaFilePath);
//
//	/// @brief ファイルの保存
//	/// @param _metaFilePath 保存先の.metaファイルパス
//	/// @return true: 保存成功, false: 保存失敗
//	bool SaveToFile(const std::string& _metaFilePath) const;
//
//	/// ==================================================
//	/// public : objects
//	/// ==================================================
//
//	Guid guid;
//	AssetType assetType;
//	std::unordered_map<std::string, std::string> properties;
//
//};



/// @brief MetaFileを生成する
/// @param _refFile .pngなどの参照ファイルパス
/// @return 生成されたMetaFileオブジェクト
//MetaFile GenerateMetaFile(const std::string& _refFile);


/*
* すべてのアセットに不随させるデータ
* 共通のデータ+アセット別の異なるデータを格納する
* ファイル形式はJSONを採用
*/


/// @brief MetaFileの共通部分
struct MetaBase {
	Guid guid;	                    /// アセットの一意な識別子
	AssetType type;                 /// アセットのタイプ
	std::string name;               /// アセットの名前
	std::vector<Guid> dependencies; /// アセットの依存関係を表すGuidのリスト
};

/// @brief Assetのタイプごとに異なるデータを格納するための構造体
/// @tparam T アセット別のMetaデータ構造体
template<typename T>
struct Meta {
	MetaBase base;
	T data;
};


/// @brief MetaBaseをファイルから読み込む
/// @param filepath 対象の.metaファイルパス
/// @return 読み込まれたMetaBaseオブジェクト
MetaBase LoadMetaBaseFromFile(const std::string& filepath);



} /// ONEngine
