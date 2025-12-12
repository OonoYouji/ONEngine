#include "MonoScriptEngine.h"

using namespace ONEngine;

/// std
#include <regex>

/// externals
#include <metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>


/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/FileSystem/FileSystem.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "InternalCalls/AddInternalMethods.h"


namespace {
	void LogCallback(const char* _log_domain, const char* _log_level, const char* _message, mono_bool _fatal, void*) {
		const char* domain = _log_domain ? _log_domain : "null";
		const char* level = _log_level ? _log_level : "null";
		const char* message = _message ? _message : "null";

		std::string log = "[" + std::string(domain) + "][" + std::string(level) + "] " + message;
		if (_fatal) log += " (fatal)";

		Console::Log(log);
	}

	void ConsoleLog(MonoString* _msg) {
		// MonoString* -> const char* 変換
		char* cstr = mono_string_to_utf8(_msg);
		Console::Log(cstr);
		mono_free(cstr);
	}

}


MonoScriptEngine::MonoScriptEngine() : domainReloadCounter_(0) {}
MonoScriptEngine::~MonoScriptEngine() = default;

MonoScriptEngine& MonoScriptEngine::GetInstance() {
	static MonoScriptEngine instance;
	return instance;
}

void MonoScriptEngine::Initialize() {

	SetEnvironmentVariableA("PATH", "Packages/mono/bin;C:/Windows/System32");
	SetEnvironmentVariableA("MONO_PATH", "Packages/mono/lib/4.5");

	/// 高速化用オプション
	const char* options[] = {
		"--optimize=all",   // JIT最適化フル
		//"--gc=sgen",        // Generational GC
		//"--llvm"            // LLVMバックエンド (ビルド時有効なら)
	};
	mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);

	/// ログ出力(任意、デバッグ時だけでもOK)
	mono_trace_set_level_string("info");
	mono_trace_set_log_handler(LogCallback, nullptr);

	/// versionの出力
	Console::Log("Mono version: " + std::string(mono_get_runtime_build_info()));

	/// Monoの検索パス設定
	mono_set_dirs("./Packages/Scripts/lib", "./Externals/mono/etc");
	mono_config_parse(nullptr);

	/// JIT初期化 (v4.x CLRターゲット)
	domain_ = mono_jit_init_version("MyDomain", "v4.0.30319");
	if (!domain_) {
		Console::LogError("Failed to initialize Mono JIT");
		return;
	}

	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::LogError("Failed to find latest assembly DLL.");
		return;
	}

	currentDllPath_ = *latestDll;
	assembly_ = mono_domain_assembly_open(domain_, currentDllPath_.c_str());
	if (!assembly_) {
		Console::LogError("Failed to load CSharpLibrary.dll");
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	if (!image_) {
		Console::LogError("Failed to get image from assembly");
		return;
	}

	RegisterFunctions();
}

void MonoScriptEngine::Finalize() {
	if (domain_) {
		mono_jit_cleanup(domain_);
		domain_ = nullptr;
	}
}

void MonoScriptEngine::RegisterFunctions() {
	/// 関数の登録
	AddComponentInternalCalls();

	AddEntityInternalCalls();

	/// log
	mono_add_internal_call("Debug::InternalConsoleLog", (void*)ConsoleLog);

	/// time
	mono_add_internal_call("Time::InternalGetDeltaTime", (void*)Time::DeltaTime);
	mono_add_internal_call("Time::InternalGetTime", (void*)Time::GetTime);
	mono_add_internal_call("Time::InternalGetUnscaledDeltaTime", (void*)Time::UnscaledDeltaTime);
	mono_add_internal_call("Time::InternalGetTimeScale", (void*)Time::TimeScale);
	mono_add_internal_call("Time::InternalSetTimeScale", (void*)Time::SetTimeScale);

	mono_add_internal_call("Mathf::LoadFile", (void*)MonoInternalMethods::LoadFile);

	/// 他のクラスの関数も登録
	AddInputInternalCalls();

	AddSceneInternalCalls();
}

void MonoScriptEngine::HotReload() {
	MonoDomain* oldDomain = domain_;
	std::string oldDllPath = currentDllPath_; // 今読み込んでるDLLのパスを保存

	domain_ = CreateReloadDomain();
	mono_domain_set(domain_, true);

	if (domain_ != oldDomain) {
		Console::Log("Created new Mono domain for hot reload.");
	} else {
		Console::Log("Reusing existing Mono domain for hot reload.");
	}

	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::LogError("Failed to find latest assembly DLL.");
		mono_domain_set(oldDomain, true);
		mono_domain_unload(domain_);
		domain_ = oldDomain;
		return;
	}

	assembly_ = mono_domain_assembly_open(domain_, latestDll->c_str());
	if (!assembly_) {
		Console::LogError("Failed to load assembly in new domain");
		mono_domain_set(oldDomain, true);
		mono_domain_unload(domain_);
		domain_ = oldDomain;
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	RegisterFunctions();

	if (oldDomain != mono_get_root_domain()) {
		mono_domain_unload(oldDomain);
	}

	currentDllPath_ = *latestDll;

	/// ScriptUpdateSystemのスクリプトのリセットを要請
	SetIsHotReloadRequest(true);

	Console::Log("Reloaded assembly successfully in new domain.");
}

std::optional<std::string> MonoScriptEngine::FindLatestDll(const std::string& _dirPath, const std::string& _baseName) {
	std::regex pattern(_baseName + R"(_(\d{8})_(\d{6})\.dll)");
	std::optional<std::string> latestFile;
	std::string latestTimestamp;

	for (const auto& entry : std::filesystem::directory_iterator(_dirPath)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		std::string filename = entry.path().filename().string();
		std::smatch match;
		if (!std::regex_match(filename, match, pattern)) {
			continue;
		}

		// match[1] → 日付（YYYYMMDD）、match[2] → 時刻（HHMMSS）
		std::string timestamp = match[1].str() + match[2].str(); // "yyyyMMddHHmmss"

		if (!latestFile || timestamp > latestTimestamp) {
			latestFile = entry.path().string();
			latestTimestamp = timestamp;
		}
	}

	return latestFile;
}

void MonoScriptEngine::ResetCS() {
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem");
		return;
	}

	MonoMethod* method = mono_class_get_method_from_name(monoClass, "DeleteEntityAll", 0);
	if (!method) {
		Console::LogError("Failed to find method: DeleteEntityAll");
		return;
	}


	/// 関数を呼び出す
	MonoObject* exc = nullptr;
	mono_runtime_invoke(method, nullptr, nullptr, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err);
		mono_free(err);
	}

}

MonoObject* MonoScriptEngine::GetEntityFromCS(const std::string& _ecsGroupName, int32_t _entityId) {
	/// MonoClassを取得
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem");
		return nullptr;
	}

	/// MonoMethodを取得
	MonoMethod* method = mono_class_get_method_from_name(monoClass, "GetEntity", 2);
	if (!method) {
		Console::LogError("Failed to find method: GetEntity");
		return nullptr;
	}

	/// 引数を設定
	MonoString* ecsGroupNameStr = mono_string_new(mono_domain_get(), _ecsGroupName.c_str());
	void* args[2];
	args[0] = ecsGroupNameStr;
	args[1] = &_entityId;

	/// MonoObjectを取得
	MonoObject* exc = nullptr;
	MonoObject* result = mono_runtime_invoke(method, nullptr, args, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err);
		mono_free(err);
		return nullptr;
	}

	if (result) {
		return result;
	}

	return nullptr;
}

MonoObject* MonoScriptEngine::GetMonoBehaviorFromCS(const std::string& _ecsGroupName, int32_t _entityId, const std::string& _behaviorName) {
	/// MonoClassを取得
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem");
		return nullptr;
	}

	/// MonoMethodを取得
	MonoMethod* method = mono_class_get_method_from_name(monoClass, "GetMonoBehavior", 3);
	if (!method) {
		Console::LogError("Failed to find method: GetEntity");
		return nullptr;
	}

	/// 引数を設定
	MonoString* ecsGroupNameStr = mono_string_new(mono_domain_get(), _ecsGroupName.c_str());
	MonoString* behaviorNameStr = mono_string_new(mono_domain_get(), _behaviorName.c_str());
	void* args[3];
	args[0] = ecsGroupNameStr;
	args[1] = &_entityId;
	args[2] = behaviorNameStr;

	/// MonoObjectを取得
	MonoObject* exc = nullptr;
	MonoObject* result = mono_runtime_invoke(method, nullptr, args, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err);
		mono_free(err);
		return nullptr;
	}

	if (result) {
		return result;
	}

	return nullptr;
}

MonoMethod* MonoScriptEngine::GetMethodFromCS(const std::string& _className, const std::string& _methodName, int _argsCount) {
	/// MonoClassを取得
	MonoClass* monoClass = mono_class_from_name(image_, "", _className.c_str());
	if (!monoClass) {
		Console::LogError("Failed to find class: " + _className);
		return nullptr;
	}

	/// クラス階層を親まで辿って検索
	for (MonoClass* current = monoClass; current != nullptr; current = mono_class_get_parent(current)) {
		MonoMethod* method = mono_class_get_method_from_name(current, _methodName.c_str(), _argsCount);
		if (method) {
			return method; // 見つかったら即返す
		}
	}

	Console::LogError("Failed to find method: " + _className + "::" + _methodName);
	return nullptr;
}

MonoDomain* MonoScriptEngine::CreateReloadDomain() {
	std::string domainName = "ReloadedDomain_" + std::to_string(domainReloadCounter_);

	MonoDomain* domain = mono_domain_create_appdomain((char*)domainName.c_str(), nullptr);
	if (!domain) {
		Console::LogError("Failed to create Mono domain for hot reload: " + domainName);
		return nullptr;
	}

	return domain;
}



MonoDomain* MonoScriptEngine::Domain() const {
	return domain_;
}

MonoImage* MonoScriptEngine::Image() const {
	return image_;
}

MonoAssembly* MonoScriptEngine::Assembly() const {
	return assembly_;
}

void MonoScriptEngine::SetIsHotReloadRequest(bool _request) {
	isHotReloadRequest_ = _request;
}

bool MonoScriptEngine::GetIsHotReloadRequest() const {
	return isHotReloadRequest_;
}



MonoMethod* MonoScriptEngineUtils::FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount) {
	while (_class) {
		MonoMethod* method = mono_class_get_method_from_name(_class, _methodName, _paramCount);
		if (method)
			return method;
		_class = mono_class_get_parent(_class);
	}
	return nullptr;
}
