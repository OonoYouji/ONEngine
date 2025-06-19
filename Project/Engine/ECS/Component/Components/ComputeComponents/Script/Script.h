#pragma once

/// std
#include <string>

/// externals
#include "mono/jit/jit.h"

/// engine
#include "Engine/ECS/Component/Components/Interface/IComponent.h"


/// ///////////////////////////////////////////////////
/// スクリプトコンポーネント
/// ///////////////////////////////////////////////////
class Script : public IComponent {
	friend class MonoScriptEngine;
	friend class ScriptUpdateSystem;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Script();
	~Script() override;

	void SetScript(const std::string& _scriptName);

	void ResetScript();

	const std::string& GetScriptName() const;

private:
	std::string scriptName_;

	uint32_t gcHandle_;
	MonoClass* monoClass_;
	MonoObject* instance_;

	MonoMethod* initMethod_ = nullptr;
	MonoMethod* updateMethod_ = nullptr;
};

