#pragma once

/// std
#include <string>

/// externals
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"

/// ///////////////////////////////////////////////////
/// monoを使ったC#スクリプトエンジン
/// ///////////////////////////////////////////////////
class MonoScriptEngine {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MonoScriptEngine();
	~MonoScriptEngine();

	void Initialize();

	void MakeScript(class Script* _script, const std::string& _scriptName);

	void RegisterFunctions();
private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoDomain* domain_;
	MonoImage* image_;
	MonoAssembly* assembly_ = nullptr;
};

