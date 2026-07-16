#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>
#include <magic_enum/magic_enum.hpp>

namespace ONEngine::Asset {

/// ////////////////////////////////////////////////////
/// アセットの種類
/// ////////////////////////////////////////////////////
enum class AssetType {
	None,
	Texture,
	Mesh,
	Audio,
	Material,
	Shader,
	AnimationClip,
	Count
};


/**
 * @brief ファイル拡張子が指定されたアセットタイプに適合するかどうかを判定します。
 * @param _extension 判定対象の拡張子（例: ".png", ".obj" 等）
 * @param _type 比較対象のアセットタイプ
 * @return 拡張子がアセットタイプに対応している場合はtrue、それ以外はfalse
 */
bool CheckAssetType(const std::string& _extension, AssetType _type);

/**
 * @brief ファイル拡張子がテンプレート指定されたアセットタイプに適合するかどうかを判定します。
 * @tparam T 判定対象のアセットタイプ
 * @param _extension 判定対象の拡張子（例: ".png", ".obj" 等）
 * @return 拡張子がアセットタイプに対応している場合はtrue、それ以外はfalse
 */
template <AssetType T>
bool CheckAssetType(const std::string& _extension) {
	return CheckAssetType(_extension, T);
}

/**
 * @brief ファイル拡張子から対応するアセットタイプ（AssetType）を取得します。
 * @param _extension アセットファイルの拡張子（例: ".png"）
 * @return 対応するAssetType（該当なしの場合はAssetType::None）
 */
AssetType GetAssetTypeFromExtension(const std::string& _extension);



/**
 * @brief JSONオブジェクトからAssetType列挙型へのデシリアライズを行います。
 * @param j jsonオブジェクト
 * @param type 変換先AssetType列挙型の参照
 */
inline void from_json(const nlohmann::json& j, AssetType& type) {
	if(j.is_string()) {
		auto opt = magic_enum::enum_cast<AssetType>(j.get<std::string>());
		type = opt.value_or(AssetType::None);
	} else if(j.is_number()) {
		type = static_cast<AssetType>(j.get<int>());
	} else {
		type = AssetType::None;
	}
}

/**
 * @brief AssetType列挙型からJSONオブジェクトへのシリアライズを行います。
 * @param j jsonオブジェクト
 * @param type 変換元AssetType列挙型
 */
inline void to_json(nlohmann::json& j, const AssetType& type) {
	j = std::string(magic_enum::enum_name(type));
}



} /// namespace ONEngine