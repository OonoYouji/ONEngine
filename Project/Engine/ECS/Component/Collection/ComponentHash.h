#pragma once

/// std
#include <string>

#include "../Components/Interface/IComponent.h"


namespace ONEngine {

/**
 * @brief コンポーネントのポインタから実行時型情報（RTTI）をもとに名前文字列を取得します。
 * @param _component 対象とするコンポーネントインスタンスポインタ
 * @return クラス名文字列（名前空間等は除外されます）
 */
inline std::string GetComponentTypeName(const IComponent* _component) {
	std::string name = typeid(*_component).name();
	size_t pos = name.find_last_of(':');
	if (pos != std::string::npos) {
		name = name.substr(pos + 1);
	} else {
		// handle space if no colon (e.g. "class MyComp")
		pos = name.find_last_of(' ');
		if (pos != std::string::npos) {
			name = name.substr(pos + 1);
		}
	}
	return name;
}

/**
 * @brief テンプレート引数に指定されたコンポーネント型から名前文字列を取得します。
 * @tparam T 対象とするコンポーネント型
 * @return クラス名文字列
 */
template <IsComponent T>
inline std::string GetComponentTypeName() {
	std::string name = typeid(T).name();
	size_t pos = name.find_last_of(':');
	if (pos != std::string::npos) {
		name = name.substr(pos + 1);
	} else {
		pos = name.find_last_of(' ');
		if (pos != std::string::npos) {
			name = name.substr(pos + 1);
		}
	}
	return name;
}

/**
 * @brief コンポーネント名文字列から一意なハッシュ値（std::hash）を算出します。
 * @param _name コンポーネント名
 * @return 算出されたハッシュ値
 */
inline size_t GetComponentHash(const std::string& _name) {
	return std::hash<std::string>()(_name);
}

/**
 * @brief テンプレート引数に指定されたコンポーネント型から一意なハッシュ値を算出します。
 * @tparam T 対象とするコンポーネント型
 * @return 算出されたハッシュ値
 */
template <IsComponent T>
inline size_t GetComponentHash() {
	return GetComponentHash(GetComponentTypeName<T>());
}

} /// ONEngine