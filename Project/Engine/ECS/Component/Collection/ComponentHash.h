#pragma once

/// std
#include <string>


namespace ONEngine {

/// @brief Componentの名前からハッシュ値を取得する
/// @param _name Componentの名前
/// @return Hash値
inline size_t GetComponentHash(const std::string& _name) {
	return std::hash<std::string>()(_name);
}

/// @brief Componentの型からハッシュ値を取得する
/// @tparam T Componentの型
/// @return Hash値
template <typename T>
inline size_t GetComponentHash() {
	std::string name = typeid(T).name();
	if (name.find("class ONEngine::") == 0) {
		name = name.substr(strlen("class ONEngine::"));
	}

	return GetComponentHash(name);
}

/// @brief Componentの型から名前を取得する
/// @tparam T Componentの型
/// @return クラス名
template <typename T>
inline std::string GetComponentTypeName() {
	std::string name = typeid(T).name();
	if (name.find("class ONEngine::") == 0) {
		name = name.substr(strlen("class ONEngine::"));
	}
	return name;
}

} /// ONEngine