#include "MonoScriptEngine.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"

namespace {

	EntityComponentSystem* gECS = nullptr;


	Transform* InternalGetTransform(int _id) {
		return gECS->GetComponent<Transform>(_id);
	}

	void InternalSetTransform(int _id, Transform* _transform) {
		if (!_transform) {
			Console::Log("Transform pointer is null");
			return;
		}
		Transform* transform = gECS->GetComponent<Transform>(_id);
		if (transform) {

			transform->enable = _transform->enable;
			transform->position = _transform->position;
			transform->rotate = _transform->rotate;
			transform->scale = _transform->scale;

			//*transform = *_transform; /// 変数のコピー
		} else {
			Console::Log("Transform not found for entity ID: " + std::to_string(_id));
		}
	}

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

	assembly_ = mono_domain_assembly_open(domain_, "./Packages/Scripts/CSharpLibrary.dll");
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

void MonoScriptEngine::RegisterFunctions() {
	mono_add_internal_call("MonoBehavior::InternalGetTransform", (void*)InternalGetTransform);
	mono_add_internal_call("MonoBehavior::InternalSetTransform", (void*)InternalSetTransform);

	/// 他のクラスの関数も登録
	Input::RegisterMonoFunctions();

}

void MonoScriptEngine::HotReload() {
	// 旧ドメインを保持しておく
	MonoDomain* oldDomain = domain_;

	// 新ドメインを作成
	domain_ = mono_domain_create_appdomain((char*)"ReloadedDomain", nullptr);
	mono_domain_set(domain_, true);

	// DLL読み込み
	assembly_ = mono_domain_assembly_open(domain_, "./Packages/Scripts/CSharpLibrary.dll");
	if (!assembly_) {
		Console::Log("Failed to load assembly in new domain");
		mono_domain_set(oldDomain, true);
		mono_domain_unload(domain_);
		domain_ = oldDomain;
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	RegisterFunctions();

	// 古いドメインを破棄（安全にやるなら後でタイミング管理して）
	if (oldDomain != mono_get_root_domain()) {
		mono_domain_unload(oldDomain);
	}

	Console::Log("Reloaded assembly successfully in new domain.");
}
