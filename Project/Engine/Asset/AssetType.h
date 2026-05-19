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

inline void to_json(nlohmann::json& j, const AssetType& type) {
	j = std::string(magic_enum::enum_name(type));
}



} /// namespace ONEngine