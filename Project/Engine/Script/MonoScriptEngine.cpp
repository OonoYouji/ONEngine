#include "MonoScriptEngine.h"

/// std
#include <regex>

/// externals
//#include <metadata/debug-mono-symfile.h>
#include <metadata/mono-config.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

namespace {
	MonoScriptEngine* gMonoScriptEngine = nullptr;

	void LogCallback(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data) {
		Console::Log(std::string("[") + log_domain + "][" + log_level + "] " + message + (fatal ? " (fatal)" : ""));
	}

	MonoMethod* FindMethodInClassOrParents(MonoClass* _class, const char* _methodName, int _paramCount) {
		while (_class) {
			MonoMethod* method = mono_class_get_method_from_name(_class, _methodName, _paramCount);
			if (method)
				return method;
			_class = mono_class_get_parent(_class);
		}
		return nullptr;
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
		Console::Log("MonoScriptEngine pointer is null");
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

	mono_trace_set_level_string("debug");
	mono_trace_set_log_handler(LogCallback, nullptr);

	// Mono の検索パス設定（必ず先）
	mono_set_dirs("./Externals/mono/lib", "./Externals/mono/etc");

	mono_config_parse(nullptr);
	// debugサポートの初期化
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);

	// デバッグオプションを渡す
	const char* options[] = {
		"--soft-breakpoints"   // ソフトブレークポイントを有効にする
		//"--debugger-agent=transport=dt_socket,address=127.0.0.1:55555,server=y,suspend=n"
	};
	mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);


	// JIT初期化
	domain_ = mono_jit_init_version("MyDomain", "v4.0.30319");
	if (!domain_) {
		Console::Log("Failed to initialize Mono JIT");
		return;
	}

	mono_debug_domain_create(domain_);


	// DLLを開く
	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::Log("Failed to find latest assembly DLL.");
		return;
	}

	currentDllPath_ = *latestDll;
	assembly_ = mono_domain_assembly_open(domain_, currentDllPath_.c_str());
	if (!assembly_) {
		Console::Log("Failed to load CSharpLibrary.dll");
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	//mono_debug_open_image();
	if (!image_) {
		Console::Log("Failed to get image from assembly");
		return;
	}

	RegisterFunctions();
}


void MonoScriptEngine::MakeScript(Script* _script, const std::string& _scriptName) {
	if (!_script) {
		Console::Log("Script pointer is null");
		return;
	}

	/// MonoImageから指定されたクラスを取得(namespaceは""で省略)
	MonoClass* monoClass = mono_class_from_name(image_, "", _scriptName.c_str());
	if (!monoClass) {
		Console::Log("Failed to find class: " + _scriptName);
		return;
	}

	/// クラスのインスタンスを生成
	MonoObject* obj = mono_object_new(domain_, monoClass);
	mono_runtime_object_init(obj); /// クラスの初期化、コンストラクタをイメージ
	uint32_t gcHandle = mono_gchandle_new(obj, false); /// GCハンドルを取得（必要に応じて）

	/// Initializeメソッドを取得
	MonoMethod* initMethod = FindMethodInClassOrParents(monoClass, "InternalInitialize", 1);
	if (!initMethod) {
		Console::Log("Failed to find method Initialize in class: " + _scriptName);
		//return;
	}

	/// Updateメソッドを取得
	MonoMethod* updateMethod = FindMethodInClassOrParents(monoClass, "Update", 0);
	if (!updateMethod) {
		Console::Log("Failed to find method Update in class: " + _scriptName);
		//return;
	}


	/// スクリプトのメンバ変数に設定
	_script->gcHandle_ = gcHandle;
	_script->monoClass_ = monoClass;
	_script->instance_ = obj;
	_script->initMethod_ = initMethod;
	_script->updateMethod_ = updateMethod;

	/// c#側のEntityのidを設定
	MonoClassField* field = mono_class_get_field_from_name(_script->monoClass_, "entityId");
	uint32_t id = _script->GetOwner()->GetId();
	mono_field_set_value(_script->instance_, field, &id);


	/// 初期化の呼び出し
	if (initMethod && obj) {
		IEntity* owner = _script->GetOwner();
		uint32_t id = owner->GetId();
		void* args[1];
		args[0] = &id;

		MonoObject* exc = nullptr;
		mono_runtime_invoke(initMethod, obj, args, &exc);

		if (exc) {
			char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
			Console::Log(std::string("Exception thrown: ") + err);
			mono_free(err);
		}

	}

	//if (updateMethod && obj) {
	//	mono_runtime_invoke(updateMethod, obj, nullptr, nullptr);
	//}


}

//void MonoScriptEngine::RegisterEntity(Script* _script) {
//
//	/// c#側のEntityのidを設定
//	MonoClassField* field = mono_class_get_field_from_name(_script->monoClass_, "entityId");
//	uint32_t id = _script->GetOwner()->GetId();
//	mono_field_set_value(_script->instance_, field, &id);
//
//}

void MonoScriptEngine::RegisterFunctions() {
	/// 関数の登録

	/// transformの get set
	mono_add_internal_call("Transform::InternalGetTransform", (void*)InternalGetTransform);
	mono_add_internal_call("Transform::InternalGetPosition", (void*)InternalGetPosition);
	mono_add_internal_call("Transform::InternalGetRotate", (void*)InternalGetRotate);
	mono_add_internal_call("Transform::InternalGetScale", (void*)InternalGetScale);
	mono_add_internal_call("Transform::InternalSetPosition", (void*)InternalSetPosition);
	mono_add_internal_call("Transform::InternalSetRotate", (void*)InternalSetRotate);
	mono_add_internal_call("Transform::InternalSetScale", (void*)InternalSetScale);

	/// component
	mono_add_internal_call("Entity::InternalAddComponent", (void*)InternalAddComponent);

	/// log
	mono_add_internal_call("Log::InternalConsoleLog", (void*)ConsoleLog);

	/// time
	mono_add_internal_call("Time::InternalGetDeltaTime", (void*)Time::DeltaTime);
	mono_add_internal_call("Time::InternalGetTime", (void*)Time::GetTime);
	mono_add_internal_call("Time::InternalGetUnscaledDeltaTime", (void*)Time::UnscaledDeltaTime);
	mono_add_internal_call("Time::InternalGetTimeScale", (void*)Time::TimeScale);
	mono_add_internal_call("Time::InternalSetTimeScale", (void*)Time::SetTimeScale);

	//mono_add_internal_call("Entity::InternalSetTransform", (void*)InternalSetTransform);
	//MonoObject* InternalGetTransform(uint32_t _entityId);
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
		Console::Log("Failed to find latest assembly DLL.");
		mono_domain_set(oldDomain, true);
		mono_domain_unload(domain_);
		domain_ = oldDomain;
		return;
	}

	assembly_ = mono_domain_assembly_open(domain_, latestDll->c_str());
	if (!assembly_) {
		Console::Log("Failed to load assembly in new domain");
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
			Console::Log("Failed to delete old DLL: " + ec.message());
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



MonoDomain* MonoScriptEngine::Domain() const {
	return domain_;
}

MonoImage* MonoScriptEngine::Image() const {
	return image_;
}

MonoAssembly* MonoScriptEngine::Assembly() const {
	return assembly_;
}
