#pragma once

#include <string>

/// @brief Componentの名前からハッシュ値を取得する
/// @param _name Componentの名前
/// @return Hash値
static size_t GetComponentHash(const std::string& _name) {
	return std::hash<std::string>()(_name);
}

/// @brief Componentの型からハッシュ値を取得する
/// @tparam T Componentの型
/// @return Hash値
template <typename T>
static size_t GetComponentHash() {
	std::string name = typeid(T).name();
	if (name.find("class ") == 0) {
		name = name.substr(6);
	}

	return GetComponentHash(name);
}