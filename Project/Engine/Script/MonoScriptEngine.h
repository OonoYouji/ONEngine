#pragma once

/// std
#include <string>
#include <optional>

/// externals
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/debug-helpers.h"
//#include "mono/metadata/debug-mono-symfile.h"
#include "mono/utils/mono-logger.h"

void SetMonoScriptEnginePtr(class MonoScriptEngine* _engine);
MonoScriptEngine* GetMonoScriptEnginePtr();


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

	//void RegisterEntity(class Script* _script, const std::string& _csName);

	void RegisterFunctions();

	void HotReload();

	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

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

