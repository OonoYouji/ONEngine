#pragma once

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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoDomain* domain;
	MonoImage* image;
	MonoAssembly* assembly = nullptr;
};

