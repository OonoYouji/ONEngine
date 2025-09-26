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

	bool Contains(const std::string& _scriptName) const;
	void AddScript(const std::string& _scriptName);
	void RemoveScript(const std::string& _scriptName);

	const std::string& GetScriptName(size_t _index) const;
	std::vector<std::string> GetScriptNames() const;

	const std::vector<ScriptData>& GetScriptDataList() const;
	std::vector<ScriptData>& GetScriptDataList();
	ScriptData* GetScriptData(const std::string& _scriptName);

	void SetEnable(const std::string& _scriptName, bool _enable);
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
