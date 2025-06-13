#include "MonoScriptEngine.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"


MonoScriptEngine::MonoScriptEngine() {}
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

	assembly_ = mono_domain_assembly_open(domain_, "./Assets/Scripts/CSharpLibrary.dll");
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

	/// 先に定義しておく
	MonoMethodDesc* desc = nullptr;

	/// Updateメソッドを取得
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
	_script->monoClass_ = monoClass;
	_script->instance_ = obj;
	_script->initMethod_ = initMethod;
	_script->updateMethod_ = updateMethod;
}

void MonoScriptEngine::RegisterFunctions() {

}
