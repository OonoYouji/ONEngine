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


/**
 * @struct MetaBase
 * @brief すべてのアセットのメタファイル（.meta）に共通して含まれる基底データ構造体
 */
struct MetaBase {
	Guid guid;	                    ///< アセットの一意な識別子
	AssetType type;                 ///< アセットのタイプ（Texture, Material等）
	std::string name;               ///< アセットの名前
	std::vector<Guid> dependencies; ///< アセットの依存関係を表すGuidのリスト
};

/**
 * @struct Meta
 * @brief アセットタイプ別のメタデータ構造体。共通部（base）と固有部（data）を持ちます。
 * @tparam T アセット固有のメタデータ型
 */
template<typename T>
struct Meta {
	MetaBase base; ///< メタデータ共通データ
	T data;        ///< 各アセットタイプ固有データ
};


/**
 * @brief 指定されたメタファイルパスから共通メタデータ（MetaBase）をロードします。存在しない場合は新規生成（UUID等の割り当て）を行います。
 * @param filepath 対象の .meta ファイルパス
 * @param assetPath アセット本体のファイルパス
 * @return 取得した（または新規に作成・保存された）MetaBaseオブジェクト
 */
MetaBase LoadOrGenerateMetaBase(const std::string& filepath, const std::string& assetPath);

/**
 * @brief メタデータ（共通部および固有部）を指定ファイルパスにJSON形式で保存します。
 * @param filepath 保存先の .meta ファイルパス
 * @param metaBase 共通のメタデータ
 * @param jMetaData アセット固有のメタデータ（JSON形式のシリアライズデータ）
 */
void SaveMetaToFile(const std::string& filepath, const MetaBase& metaBase, const nlohmann::json& jMetaData);



} /// ONEngine
