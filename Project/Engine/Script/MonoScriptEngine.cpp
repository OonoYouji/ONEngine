#include "MonoScriptEngine.h"

/// std
#include <regex>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

namespace {

	EntityComponentSystem* gECS = nullptr;
	MonoScriptEngine* gMonoScriptEngine = nullptr;

	//Transform* InternalGetTransform(int _id) {
	//	return gECS->GetComponent<Transform>(_id);
	//}

	//void InternalSetTransform(int _id, Transform* _transform) {
	//	if (!_transform) {
	//		Console::Log("Transform pointer is null");
	//		return;
	//	}
	//	Transform* transform = gECS->GetComponent<Transform>(_id);
	//	if (transform) {

	//		transform->enable = _transform->enable;
	//		transform->position = _transform->position;
	//		transform->rotate = _transform->rotate;
	//		transform->scale = _transform->scale;

	//		//*transform = *_transform; /// 変数のコピー
	//	} else {
	//		Console::Log("Transform not found for entity ID: " + std::to_string(_id));
	//	}
	//}

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


MonoScriptEngine::MonoScriptEngine(EntityComponentSystem* _ecs) {
	gECS = _ecs;
}
MonoScriptEngine::~MonoScriptEngine() {
	if (domain_) {
		mono_jit_cleanup(domain_);
		domain_ = nullptr;
	}
}

void MonoScriptEngine::Initialize() {
	mono_set_dirs("./Externals/mono/lib", "./Externals/mono/etc");
	domain_ = mono_jit_init("MyDomain");
	if (!domain_) {
		Console::Log("Failed to initialize Mono JIT");
		return;
	}

	// DLL名を自動検索
	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::Log("Failed to find latest assembly DLL.");
		return;
	}

	currentDllPath_ = *latestDll; // 最新のDLLパスを保存
	assembly_ = mono_domain_assembly_open(domain_, currentDllPath_.c_str());
	if (!assembly_) {
		Console::Log("Failed to load CSharpLibrary.dll");
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	if (!image_) {
		Console::Log("Failed to get image from assembly");
		return;
	}


	/// 関数を登録
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

	/// 先に定義しておく
	MonoMethodDesc* desc = nullptr;

	/// Initializeメソッドを取得
	desc = mono_method_desc_new(":Initialize()", false);
	MonoMethod* initMethod = mono_method_desc_search_in_class(desc, monoClass);
	mono_method_desc_free(desc);
	if (!initMethod) {
		Console::Log("Failed to find method Initialize in class: " + _scriptName);
		return;
	}

	/// Updateメソッドを取得
	desc = mono_method_desc_new(":Update()", false);
	MonoMethod* updateMethod = mono_method_desc_search_in_class(desc, monoClass);
	mono_method_desc_free(desc);

	if (!updateMethod) {
		Console::Log("Failed to find method Update in class: " + _scriptName);
		return;
	}

	if (initMethod && obj) {
		mono_runtime_invoke(initMethod, obj, nullptr, nullptr);
	}


	/// スクリプトのメンバ変数に設定
	_script->gcHandle_ = gcHandle;
	_script->monoClass_ = monoClass;
	_script->instance_ = obj;
	_script->initMethod_ = initMethod;
	_script->updateMethod_ = updateMethod;
}

void MonoScriptEngine::RegisterEntity(IEntity* _entity) {
	/// 

	MonoClass* monoClass = mono_class_from_name(image_, "", _entity->GetName().c_str());
	if (!monoClass) {
		Console::Log("Failed to find class: " + _entity->GetName());
		return;
	}

	MonoObject* instance = mono_object_new(domain_, monoClass);
	mono_runtime_object_init(instance); /// クラスの初期化、コンストラクタをイメージ

	/// c#側のEntityのnativeHandleを設定
	MonoClassField* field = mono_class_get_field_from_name(monoClass, "nativeHandle");
	mono_field_set_value(instance, field, &_entity);


}

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
	std::regex pattern(_baseName + R"(_\d{8}_\d{6}\.dll)");
	std::optional<std::string> latestFile;
	std::chrono::file_clock::time_point latestTime;

	for (const auto& entry : std::filesystem::directory_iterator(_dirPath)) {
		if (!entry.is_regular_file()) continue;

		std::string filename = entry.path().filename().string();
		if (!std::regex_match(filename, pattern)) continue;

		auto ftime = std::filesystem::last_write_time(entry);
		if (!latestFile || ftime > latestTime) {
			latestFile = entry.path().string();
			latestTime = ftime;
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
