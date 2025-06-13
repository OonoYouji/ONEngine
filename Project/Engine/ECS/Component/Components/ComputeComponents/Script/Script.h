#pragma once

/// std
#include <string>

/// externals
#include "mono/jit/jit.h"

/// engine
#include "Engine/ECS/Component/Components/Interface/IComponent.h"

void SetMonoScriptEnginePtr(class MonoScriptEngine* _engine);


/// ///////////////////////////////////////////////////
/// スクリプトコンポーネント
/// ///////////////////////////////////////////////////
class Script : public IComponent {
	friend class MonoScriptEngine;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Script();
	~Script() override;

	void SetScript(const std::string& _scriptName);

private:
	MonoClass* monoClass_;
	MonoObject* instance_;

	MonoMethod* initMethod_ = nullptr;
	MonoMethod* updateMethod_ = nullptr;
};

