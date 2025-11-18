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
#include "Engine/Core/Utility/Tools/Log.h"

/// Json変換
void from_json(const nlohmann::json& _j, class Variables& _g);
void to_json(nlohmann::json& _j, const class Variables& _g);


/// ///////////////////////////////////////////////////
/// 変数component
/// ///////////////////////////////////////////////////
class Variables : public IComponent {
	friend void from_json(const nlohmann::json& _j, Variables& _v);
	friend void to_json(nlohmann::json& _j, const Variables& _v);
public:
	/// ================================================
	/// public : sub class
	/// ================================================

	enum class VarType {
		kInt,
		kFloat,
		kBool,
		kString,
		kVector2,
		kVector3,
		kVector4,
		Unknown
	};

	using Var = std::variant<int, float, bool, std::string, Vector2, Vector3, Vector4>;


	/// @brief 変数のグループ、スクリプトごとに使用する予定
	struct Group {
		std::string name; ///< グループ名
		std::map<std::string, size_t> keyMap_;
		std::vector<Var> variables; ///< グループに属する変数

		template <typename T = Var>
		void Add(const std::string& _name, const T& _value) {
			if (keyMap_.contains(_name)) {
				variables[keyMap_[_name]] = _value;
			}

			keyMap_[_name] = variables.size();
			variables.emplace_back(_value);
		}

		template <typename T>
		T& Get(const std::string& _name) {
			return std::get<T>(variables[keyMap_.at(_name)]);
		}

		template <typename T>
		const T& Get(const std::string& _name) const {
			return std::get<T>(variables[keyMap_.at(_name)]);
		}

		const Var& Get(const std::string& _name) const;

		bool Has(const std::string& _name) const;


	};


public:
	/// ================================================
	/// public : methods
	/// ================================================


	Variables();
	~Variables() override;


	/// @brief jsonを読み込んで変数を設定する
	/// @param _path 読み込むjsonファイルのパス
	void LoadJson(const std::string& _path);

	/// @brief 変数をjsonに保存する
	/// @param _path 保存するjsonファイルのパス
	void SaveJson(const std::string& _path);


	/// @brief スクリプト内の変数を登録する
	void RegisterScriptVariables();

	/// @brief スクリプト内の変数を再読み込みする
	void ReloadScriptVariables();

	/// @brief スクリプトに変数の値を設定する
	/// @param _scriptName 対象のスクリプト名
	void SetScriptVariables(const std::string& _scriptName);


	/// @brief 変数のグループ(スクリプト単位)を追加する
	/// @param _name グループ名
	/// @return GroupのIndex
	size_t AddGroup(const std::string& _name);

	/// @brief 指定した名前に対応するグループを取得する
	/// @param _name 取得するグループの名前。
	/// @return 指定された名前に対応するGroup
	const Group& GetGroup(const std::string& _name) const;

	/// @brief 引数のGroupを持っているか
	/// @param _name Group名
	/// @return true: 持っている false: 持っていない
	bool HasGroup(const std::string& _name) const;


	/// @brief グループのキーのマップを取得する
	const std::unordered_map<std::string, size_t>& GetGroupKeyMap() const;

	/// @brief グループの配列をすべて得る
	const std::vector<Group>& GetGroups() const;

private:
	/// ================================================
	/// private : objects
	/// ================================================

	std::unordered_map<std::string, size_t> groupKeyMap_; /// グループ名とインデックスのマップ
	std::vector<Group> groups_; /// 変数のグループ
};


/// ==================================================
/// public : methods
/// ==================================================


/// @brief ComponentのDebug
namespace COMP_DEBUG {
	/// @brief Variableをデバッグする
	/// @param _variables 対象のポインタ
	void VariablesDebug(Variables* _variables);
}