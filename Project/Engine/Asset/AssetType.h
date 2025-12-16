#pragma once

/// std
#include <string>

namespace ONEngine {

/// ////////////////////////////////////////////////////
/// アセットの種類
/// ////////////////////////////////////////////////////
enum class AssetType {
	None,
	Texture,
	Mesh,
	Audio,
	Material,
	Count
};


/// @brief 入力された拡張子と第二引数のTypeが同一タイプかチェックする
/// @param _extension 拡張子
/// @param _type 確認したいType
/// @return true: 同一のType   false: Typeの不一致 
bool CheckAssetType(const std::string& _extension, AssetType _type);

/// @brief 入力された拡張子とテンプレート引数のTypeが同一タイプかチェックする
/// @tparam T チェックしたいAssetType
/// @param _extension 確認対象の拡張子
/// @return true: 同一のType   false: Typeの不一致
template <AssetType T>
bool CheckAssetType(const std::string& _extension) {
	return CheckAssetType(_extension, T);
}

/// @brief 拡張子からAssetTypeを取得する
/// @param _extension 拡張子
/// @return AssetType
AssetType GetAssetTypeFromExtension(const std::string& _extension);

} /// namespace ONEngine