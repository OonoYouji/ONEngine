#include "MonoScriptEngine.h"

/// std
#include <regex>

/// externals
//#include <metadata/debug-mono-symfile.h>
#include <metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

#include "InternalCalls/AddComponentInternalCalls.h"

namespace {
	MonoScriptEngine* gMonoScriptEngine = nullptr;

	void LogCallback(const char* _log_domain, const char* _log_level, const char* _message, mono_bool _fatal, void* _user_data) {
		Console::Log(std::string("[") + _log_domain + "][" + _log_level + "] " + _message + (_fatal ? " (fatal)" : ""));
	}

	void ConsoleLog(MonoString* _msg) {
		// MonoString* -> const char* 変換
		char* cstr = mono_string_to_utf8(_msg);
		Console::Log(cstr);
		mono_free(cstr);
	}

}

void SetMonoScriptEnginePtr(MonoScriptEngine* _engine) {
	gMonoScriptEngine = _engine;
	if (!gMonoScriptEngine) {
		Console::LogWarning("MonoScriptEngine pointer is null");
	}
}

MonoScriptEngine* GetMonoScriptEnginePtr() {
	return gMonoScriptEngine;
}


MonoScriptEngine::MonoScriptEngine() {}
MonoScriptEngine::~MonoScriptEngine() {
	if (domain_) {
		mono_jit_cleanup(domain_);
		domain_ = nullptr;
	}
}

void MonoScriptEngine::Initialize() {

	_putenv("MONO_ENV_OPTIONS=--debug");

	mono_trace_set_level_string("debug");
	mono_trace_set_log_handler(LogCallback, nullptr);

	// versionの出力
	Console::Log("Mono version: " + std::string(mono_get_runtime_build_info()));

	// Mono の検索パス設定（必ず先）
	mono_set_dirs("./Packages/Scripts/lib", "./Externals/mono/etc");
	mono_config_parse(nullptr);


	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	// JIT初期化
	domain_ = mono_jit_init("MyDomain");
	if (!domain_) {
		Console::LogError("Failed to initialize Mono JIT");
		return;
	}

	mono_debug_domain_create(domain_);


	// DLLを開く
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
	//mono_debug_open_image();
	if (!image_) {
		Console::LogError("Failed to get image from assembly");
		return;
	}

	RegisterFunctions();
}

void MonoScriptEngine::MakeScript(Script* _comp, Script::ScriptData* _script, const std::string& _scriptName) {
	Console::Log("MonoScriptEngine::MakeScript: component owner: \"" + _scriptName + "\", script name: \"" + _scriptName + "\"");

	if (!_script) {
		Console::LogWarning("Script pointer is null");
		return;
	}

	_script->scriptName = _scriptName;


	/// ownerがなければ今後の処理ができないので、早期リターン
	if (!_comp->GetOwner()) {
		Console::LogError("Script owner is null. Cannot create script instance.");
		return;
	}


	/// MonoImageから指定されたクラスを取得(namespaceは""で省略)
	MonoClass* monoClass = mono_class_from_name(image_, "", _scriptName.c_str());
	if (!monoClass) {
		Console::LogError("Failed to find class: " + _scriptName);
		return;
	}

	/// クラスのインスタンスを生成
	MonoObject* obj = mono_object_new(domain_, monoClass);
	mono_runtime_object_init(obj); /// クラスの初期化、コンストラクタをイメージ
	if (!obj) {
		Console::LogError("Failed to create instance of class: " + _scriptName);
		return;
	}

	uint32_t gcHandle = mono_gchandle_new(obj, false); /// GCハンドルを取得（必要に応じて）


	/// InternalInitialize( Awake(内部で呼んでいる) )メソッドを取得
	MonoMethod* internalInitMethod = FindMethodInClassOrParents(monoClass, "InternalInitialize", 1);
	if (!internalInitMethod) {
		Console::LogError("Failed to find method InternalInitialize in class: " + _scriptName);
	}

	/// Initializeメソッドを取得
	MonoMethod* initMethod = FindMethodInClassOrParents(monoClass, "Initialize", 0);
	if (!initMethod) {
		Console::LogError("Failed to find method Initialize in class: " + _scriptName);
	}

	/// Updateメソッドを取得
	MonoMethod* updateMethod = FindMethodInClassOrParents(monoClass, "Update", 0);
	if (!updateMethod) {
		Console::LogError("Failed to find method Update in class: " + _scriptName);
	}


	/// collision イベントメソッドを取得
	_script->collisionEventMethods[0] = FindMethodInClassOrParents(monoClass, "OnCollisionEnter", 1);
	_script->collisionEventMethods[1] = FindMethodInClassOrParents(monoClass, "OnCollisionStay", 1);
	_script->collisionEventMethods[2] = FindMethodInClassOrParents(monoClass, "OnCollisionExit", 1);

	for (auto& method : _script->collisionEventMethods) {
		if (!method) {
			Console::LogError("Failed to find collision event method in class: " + _scriptName);
		}
	}

	_script->gcHandle = gcHandle;
	_script->monoClass = monoClass;
	_script->instance = obj;
	_script->internalInitMethod = internalInitMethod;
	_script->initMethod = initMethod;
	_script->updateMethod = updateMethod;
	_script->isCalledAwake = false;
	_script->isCalledInit = false;
}


void MonoScriptEngine::RegisterFunctions() {
	/// 関数の登録

	AddComponentInternalCalls();

	/// entity
	mono_add_internal_call("Entity::InternalAddComponent", (void*)InternalAddComponent);
	mono_add_internal_call("Entity::InternalGetComponent", (void*)InternalGetComponent);
	mono_add_internal_call("Entity::InternalGetName", (void*)InternalGetName);
	mono_add_internal_call("Entity::InternalSetName", (void*)InternalSetName);
	mono_add_internal_call("Entity::InternalGetChildId", (void*)InternalGetChildId);
	mono_add_internal_call("Entity::InternalGetParentId", (void*)InternalGetParentId);
	mono_add_internal_call("Entity::InternalSetParent", (void*)InternalSetParent);
	mono_add_internal_call("Entity::InternalAddScript", (void*)InternalAddScript);

	mono_add_internal_call("EntityCollection::InternalContainsEntity", (void*)InternalContainsEntity);
	mono_add_internal_call("EntityCollection::InternalGetEntityId", (void*)InternalGetEntityId);
	mono_add_internal_call("EntityCollection::InternalCreateEntity", (void*)InternalCreateEntity);
	mono_add_internal_call("EntityCollection::InternalContainsPrefabEntity", (void*)InternalContainsPrefabEntity);
	mono_add_internal_call("EntityCollection::InternalDestroyEntity", (void*)InternalDestroyEntity);

	/// log
	mono_add_internal_call("Log::InternalConsoleLog", (void*)ConsoleLog);

	/// time
	mono_add_internal_call("Time::InternalGetDeltaTime", (void*)Time::DeltaTime);
	mono_add_internal_call("Time::InternalGetTime", (void*)Time::GetTime);
	mono_add_internal_call("Time::InternalGetUnscaledDeltaTime", (void*)Time::UnscaledDeltaTime);
	mono_add_internal_call("Time::InternalGetTimeScale", (void*)Time::TimeScale);
	mono_add_internal_call("Time::InternalSetTimeScale", (void*)Time::SetTimeScale);

	/// 他のクラスの関数も登録
	Input::RegisterMonoFunctions();

}

void MonoScriptEngine::HotReload() {
	MonoDomain* oldDomain = domain_;
	std::string oldDllPath = currentDllPath_; // 今読み込んでるDLLのパスを保存

	domain_ = mono_domain_create_appdomain((char*)"ReloadedDomain", nullptr);
	mono_domain_set(domain_, true);

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

	// DLL削除
	if (!oldDllPath.empty() && std::filesystem::exists(oldDllPath)) {
		std::error_code ec;
		std::filesystem::remove(oldDllPath, ec);
		if (ec) {
			Console::LogError("Failed to delete old DLL: " + ec.message());
		} else {
			Console::Log("Old DLL deleted: " + oldDllPath);
		}
	}

	currentDllPath_ = *latestDll;

	Console::Log("Reloaded assembly successfully in new domain.");
}

std::optional<std::string> MonoScriptEngine::FindLatestDll(const std::string& _dirPath, const std::string& _baseName) {
	std::regex pattern(_baseName + R"(_(\d{8})_(\d{6})\.dll)");
	std::optional<std::string> latestFile;
	std::string latestTimestamp;

	for (const auto& entry : std::filesystem::directory_iterator(_dirPath)) {
		if (!entry.is_regular_file()) continue;

		std::string filename = entry.path().filename().string();
		std::smatch match;
		if (!std::regex_match(filename, match, pattern)) continue;

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
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityCollection");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityCollection");
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

MonoMethod* MonoScriptEngine::FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount) {
	while (_class) {
		MonoMethod* method = mono_class_get_method_from_name(_class, _methodName, _paramCount);
		if (method)
			return method;
		_class = mono_class_get_parent(_class);
	}
	return nullptr;
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
