#pragma once

/// std
#include <string>

/// ////////////////////////////////////////////////////
/// アセットの種類
/// ////////////////////////////////////////////////////
enum class AssetType {
	None,
	Texture,
	Mesh,
	Audio,
	Material
};


/// @brief 入力された拡張子と第二引数のTypeが同一タイプかチェックする
/// @param _extension 拡張子
/// @param _type 確認したいType
/// @return true: 同一のType   false: Typeの不一致 
bool CheckAssetType(const std::string& _extension, AssetType _type);


/// @brief 拡張子からAssetTypeを取得する
/// @param _extension 拡張子
/// @return AssetType
AssetType GetAssetTypeFromExtension(const std::string& _extension);