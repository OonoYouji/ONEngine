#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Script/MonoScriptEngine.h"

namespace {
	MonoScriptEngine* gMonoScriptEngine = nullptr;
}

void SetMonoScriptEnginePtr(MonoScriptEngine* _engine) {
	gMonoScriptEngine = _engine;
	if (!gMonoScriptEngine) {
		Console::Log("MonoScriptEngine pointer is null");
	}
}


Script::Script() {}
Script::~Script() {}

void Script::SetScript(const std::string& _scriptName) {
	gMonoScriptEngine->MakeScript(this, _scriptName);
}

