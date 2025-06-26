#include "Script.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Script/MonoScriptEngine.h"


Script::Script() {}
Script::~Script() {}

void Script::SetScript(const std::string& _scriptName) {
	if (scriptName_ != _scriptName) {
		scriptName_ = _scriptName;
		GetMonoScriptEnginePtr()->MakeScript(this, _scriptName);
	}
}

void Script::ResetScript() {
	GetMonoScriptEnginePtr()->MakeScript(this, scriptName_);
}

const std::string& Script::GetScriptName() const {
	return scriptName_;
}

