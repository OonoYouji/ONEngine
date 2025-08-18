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

	/// Monoの初期化
	void Initialize();

	/// CSの関数を登録
	void RegisterFunctions();

	/// CSのHotReloadを行う
	void HotReload();

	/// DLLのパスを探す
	std::optional<std::string> FindLatestDll(const std::string& _dirPath, const std::string& _baseName);

	/// C#側のリセット
	void ResetCS();

	/// C#側のEntityを取得
	MonoObject* GetEntityFromCS(const std::string& _ecsGroupName, int32_t _entityId);
	MonoObject* GetMonoBehaviorFromCS(const std::string& _ecsGroupName, int32_t _entityId, const std::string& _behaviorName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string currentDllPath_;

	MonoDomain* domain_;
	MonoImage* image_;
	MonoAssembly* assembly_ = nullptr;

	bool isHotReloadRequest_;

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	MonoDomain* Domain() const;
	MonoImage* Image() const;
	MonoAssembly* Assembly() const;

	void SetIsHotReloadRequest(bool _request);
	bool GetIsHotReloadRequest() const;

};


namespace MonoScriptEngineUtils {
	MonoMethod* FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount);
} // namespace MonoScriptEngineUtils