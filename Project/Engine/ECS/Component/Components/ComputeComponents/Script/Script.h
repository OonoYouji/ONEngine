#pragma once

/// std
#include <string>
#include <vector>
#include <array>

/// externals
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
		MonoObject* instance = nullptr;
		MonoClass* monoClass = nullptr;
		uint32_t gcHandle = 0;
		MonoMethod* initMethod = nullptr;
		MonoMethod* updateMethod = nullptr;

		std::array<MonoMethod*, 3> collisionEventMethods = {};

		bool enable = true;  ///< スクリプトの有効/無効フラグ
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Script();
	~Script() override;

	void AddScript(const std::string& _scriptName);
	void RemoveScript(const std::string& _scriptName);

	void ResetScripts();

	const std::string& GetScriptName(size_t _index) const;
	std::vector<std::string> GetScriptNames() const;

	const std::vector<ScriptData>& GetScriptDataList() const;
	std::vector<ScriptData>& GetScriptDataList();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<ScriptData> scriptDataList_;

};

namespace COMP_DEBUG {
	void ScriptDebug(Script* _script);
}