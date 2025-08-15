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
		std::array<MonoMethod*, 3> collisionEventMethods = {};
		bool enable = true;  ///< スクリプトの有効/無効フラグ
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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

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
/// mono用　internal methods
/// ///////////////////////////////////////////////////

namespace MONO_INTERNAL_METHOD {

	void InternalSetEnable(int32_t _entityId, MonoString* _scriptName, bool _enable, MonoString* _groupName);
	bool InternalGetEnable(int32_t _entityId, MonoString* _scriptName, MonoString* _groupName);
	
}