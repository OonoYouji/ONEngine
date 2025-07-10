#pragma once

/// std
#include <string>
#include <optional>

/// externals
#include <jit/jit.h>
#include <metadata/assembly.h>
#include <metadata/mono-debug.h>
#include <metadata/debug-helpers.h>
#include <utils/mono-logger.h>

void SetMonoScriptEnginePtr(class MonoScriptEngine* _engine);
MonoScriptEngine* GetMonoScriptEnginePtr();


/// engine
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

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

	void MakeScript(Script* _comp, Script::ScriptData* _script, const std::string& _scriptName);

	//void RegisterEntity(class Script* _script, const std::string& _csName);

	void RegisterFunctions();

	void HotReload();

	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

	void ResetCS();

private:

	MonoMethod* FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string currentDllPath_;

	MonoDomain* domain_;
	MonoImage* image_;
	MonoAssembly* assembly_ = nullptr;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	MonoDomain* Domain() const;
	MonoImage* Image() const;
	MonoAssembly* Assembly() const;

};

