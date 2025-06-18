#pragma once

/// std
#include <string>
#include <optional>

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

	MonoScriptEngine(class EntityComponentSystem* _ecs);
	~MonoScriptEngine();

	void Initialize();

	void MakeScript(class Script* _script, const std::string& _scriptName);

	void RegisterFunctions();

	void HotReload();

	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	MonoDomain* domain_;
	MonoImage* image_;
	MonoAssembly* assembly_ = nullptr;
};

