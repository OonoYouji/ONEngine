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
//#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////
/// 変数component
/// ////////////////////////////////////////////
class Variables : public IComponent {
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

	/// @brief jsonを読み込んで変数を設定する
	/// @param _path 読み込むjsonファイルのパス
	void LoadJson(const std::string& _path);

	/// @brief 変数をjsonに保存する
	/// @param _path 保存するjsonファイルのパス
	void SaveJson(const std::string& _path);

	void SetScriptVariables();

	size_t AddGroup(const std::string& _name);

	const Group& GetGroup(const std::string& _name) const;

	bool HasGroup(const std::string& _name) const;


	const std::unordered_map<std::string, size_t>& GetGroupKeyMap() const;

	const std::vector<Group>& GetGroups() const;

private:
	/// ================================================
	/// private : objects
	/// ================================================

	std::unordered_map<std::string, size_t> groupKeyMap_; ///< グループ名とインデックスのマップ
	std::vector<Group> groups_; ///< 変数のグループ
};


/// ==================================================
/// public : methods
/// ==================================================


namespace COMP_DEBUG {
	void VariablesDebug(Variables* _variables);
}