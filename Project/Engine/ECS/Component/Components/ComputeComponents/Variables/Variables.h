#pragma once

/// std
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>

/// external
#include <Externals/nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector2.h"


/// ////////////////////////////////////////////
/// 変数component
/// ////////////////////////////////////////////
class Variables : public IComponent {
public:
	/// ================================================
	/// public : sub class
	/// ================================================

	using Var   = std::variant<int, float, bool, std::string, Vector2, Vector3, Vector4>;
	//using Value = std::pair<std::string, Var>;


public:
	/// ================================================
	/// public : methods
	/// ================================================

		/// @brief 変数の追加
	/// @tparam T 追加する変数の型
	/// @param _name 変数名
	/// @param _value 変数の値
	template <typename T = Var>
	void Add(const std::string& _name, const T& _value);

	/// @brief 変数の取得
	/// @tparam T 変数の型
	/// @param _name 変数名
	/// @return 変数の値
	template<typename T = Var>
	T& Get(const std::string& _name);

	/// @brief 変数の取得
	/// @tparam T 変数の型
	/// @param _name 変数名
	/// @return 変数の値
	template<typename T = Var>
	const T& Get(const std::string& _name) const;

	/// @brief 変数の設定
	/// @tparam T 変数の型
	template<typename T = Var>
	void Set(const std::string& _name, const T& _value);

	/// @brief 変数を持っているか
	/// @param _name 変数名
	/// @return 変数の有無
	bool Has(const std::string& _name) const;

	/// @brief 変数の削除
	/// @param _name 変数名
	void Remove(const std::string& _name);

	/// @brief 変数のリネーム
	/// @param _oldName 変数名
	///	@param _newName 新しい変数名
	void Rename(const std::string& _oldName, const std::string& _newName);


	/// @brief jsonを読み込んで変数を設定する
	/// @param _path 読み込むjsonファイルのパス
	void LoadJson(const std::string& _path);

	/// @brief 変数をjsonに保存する
	/// @param _path 保存するjsonファイルのパス
	void SaveJson(const std::string& _path) const;


	/// @brief 変数のコンテナを取得
	/// @return 変数のコンテナ
	const std::vector<Var>& GetVariables() const { return variables_; }

	/// @brief 変数のindexコンテナを取得
	/// @return 変数のindexコンテナ
	const std::unordered_map<std::string, size_t>& GetKeyMap() const { return keyMap_; }


private:
	/// ================================================
	/// private : objects
	/// ================================================

	std::unordered_map<std::string, size_t> keyMap_;
	std::vector<Var> variables_;


};


/// ==================================================
/// public : methods
/// ==================================================

template <typename T>
void Variables::Add(const std::string& _name, const T& _value) {
	keyMap_[_name] = variables_.size();
	variables_.emplace_back(_value);
}

template<typename T>
T& Variables::Get(const std::string& _name) {
	T* ptr = nullptr;
	if (keyMap_.contains(_name)) {
		ptr = &std::get<T>(variables_[keyMap_.at(_name)]);
		return *ptr;
	}
	return *ptr;
}

template<typename T>
const T& Variables::Get(const std::string& _name) const {
	if (keyMap_.contains(_name)) {
		return std::get<T>(variables_[keyMap_.at(_name)]);
	}

	// 例外を投げるか、デフォルト値を返す
	throw std::runtime_error("Key not found: " + _name);
}

template<typename T>
void Variables::Set(const std::string& _name, const T& _value) {
	if (keyMap_.contains(_name)) {
		std::get<T>(variables_[keyMap_.at(_name)]) = _value;
	}
}