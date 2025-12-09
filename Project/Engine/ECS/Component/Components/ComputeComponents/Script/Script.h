#pragma once

/// std
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

/// externals
#include <nlohmann/json.hpp>
#include <jit/jit.h>

/// engine
#include "Engine/ECS/Component/Components/Interface/IComponent.h"


/// ///////////////////////////////////////////////////
/// スクリプトコンポーネント
/// ///////////////////////////////////////////////////
namespace ONEngine {

class Script : public IComponent {
	friend class MonoScriptEngine;
	friend class ScriptUpdateSystem;
public:

	struct ScriptData {
		std::string scriptName;
		bool isAdded = false; ///< スクリプトが追加されたかどうか
		std::array<MonoMethod*, 3> collisionEventMethods = {};

		bool enable;
		bool GetEnable(GameEntity* _entity);
		void SetEnable(GameEntity* _entity, bool _enable);
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Script();
	~Script() override;


	/// @brief スクリプトを持っているかチェックする
	/// @param _scriptName チェックする対象のスクリプト名
	/// @return true: 持っている false: 持っていない
	bool Contains(const std::string& _scriptName) const;

	/// @brief 指定されたスクリプト名を追加します。
	/// @param _scriptName 追加するスクリプトの名前
	void AddScript(const std::string& _scriptName);

	/// @brief 引数のスクリプトを削除する
	/// @param _scriptName 削除するスクリプトの名前
	void RemoveScript(const std::string& _scriptName);


	/// @brief スクリプトの名前を引数Indexから取得する
	/// @param _index Script配列のIndex
	/// @return スクリプトの名前
	const std::string& GetScriptName(size_t _index) const;

	/// @brief スクリプトの名前配列を得る
	std::vector<std::string> GetScriptNames() const;


	/// @brief スクリプトのデータのリストを取得する
	const std::vector<ScriptData>& GetScriptDataList() const;
	std::vector<ScriptData>& GetScriptDataList();
	ScriptData* GetScriptData(const std::string& _scriptName);


	/// @brief スクリプトの有効/無効を設定する
	/// @param _scriptName スクリプト名
	/// @param _enable 設定する有効/無効
	void SetEnable(const std::string& _scriptName, bool _enable);

	/// @brief スクリプトの有効/無効を取得する
	/// @param _scriptName スクリプト名
	/// @return true: 有効 false: 無効
	bool GetEnable(const std::string& _scriptName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unordered_map< std::string, size_t> scriptIndexMap_;
	std::vector<ScriptData> scriptDataList_;
	bool isAdded_;


public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// エンティティが追加されたか
	void SetIsAdded(bool _added);
	bool GetIsAdded() const;

};

namespace COMP_DEBUG {
	void ScriptDebug(Script* _script);
}



/// ///////////////////////////////////////////////////
/// json用の関数
/// ///////////////////////////////////////////////////
void from_json(const nlohmann::json& _j, Script& _s);
void to_json(nlohmann::json& _j, const Script& _s);

} /// ONEngine
