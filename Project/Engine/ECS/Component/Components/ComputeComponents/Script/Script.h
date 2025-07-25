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
		MonoMethod* internalInitMethod = nullptr;
		MonoMethod* initMethod = nullptr;
		MonoMethod* updateMethod = nullptr;

		std::array<MonoMethod*, 3> collisionEventMethods = {};

		bool enable = true;  ///< スクリプトの有効/無効フラグ

		/// update system用
		bool isCalledAwake = false;
		bool isCalledInit = false;
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

	void ResetScripts();
	void ReleaseGCHandles();
	void ReleaseGCHandle(ScriptData* _releaseScript);

	const std::string& GetScriptName(size_t _index) const;
	std::vector<std::string> GetScriptNames() const;

	const std::vector<ScriptData>& GetScriptDataList() const;
	std::vector<ScriptData>& GetScriptDataList();

	void SetEnable(const std::string& _scriptName, bool _enable);
	bool GetEnable(const std::string& _scriptName);

	void CallAwakeMethodAll();
	void CallInitMethodAll();
	void CallUpdateMethodAll();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<ScriptData> scriptDataList_;

};

namespace COMP_DEBUG {
	void ScriptDebug(Script* _script);
}

/// ///////////////////////////////////////////////////
/// mono用　internal methods
/// ///////////////////////////////////////////////////

namespace MONO_INTENRAL_METHOD {

	void InternalSetEnable(int32_t _entityId, MonoString* _scriptName, bool _enable);
	bool InternalGetEnable(int32_t _entityId, MonoString* _scriptName);
	
}