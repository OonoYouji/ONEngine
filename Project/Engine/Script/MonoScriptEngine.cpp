#include "MonoScriptEngine.h"
#include "InternalCalls/AddInternalMethods.h"

using namespace ONEngine;

/// std
#include <regex>

/// externals
#include <metadata/mono-config.h>
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>
#include <mono/metadata/tokentype.h>
#include <mono/metadata/blob.h>
#include <mono/metadata/debug-helpers.h>


/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/FileSystem/FileSystem.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/EntityComponentSystem/ComponentApplyFunc.h"
#include "InternalCalls/AddInternalMethods.h"
#include "InternalCalls/EventInternalCalls.h"

namespace {
	void LogCallback(const char* _log_domain, const char* _log_level, const char* _message, mono_bool _fatal, void*) {
		const char* domain = _log_domain ? _log_domain : "null";
		const char* level = _log_level ? _log_level : "null";
		const char* message = _message ? _message : "null";

		std::string log = "[" + std::string(domain) + "][" + std::string(level) + "] " + message;
		if (_fatal) log += " (fatal)";

		Console::Log(log, LogCategory::ScriptEngine);
	}

	void ConsoleLog(MonoString* _msg, LogCategory _category) {
		// MonoString* -> const char* 変換
		char* cstr = mono_string_to_utf8(_msg);
		Console::Log(cstr, _category);
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
	};
	mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);

	/// ログ出力(任意、デバッグ時だけでもOK)
	mono_trace_set_level_string("info");
	mono_trace_set_log_handler(LogCallback, nullptr);

	/// versionの出力
	Console::Log("Mono version: " + std::string(mono_get_runtime_build_info()), LogCategory::ScriptEngine);

	/// Monoの検索パス設定
	mono_set_dirs("./Packages/Scripts/lib", "./Externals/mono/etc");
	mono_config_parse(nullptr);

	/// JIT初期化 (v4.x CLRターゲット)
	domain_ = mono_jit_init_version("MyDomain", "v4.0.30319");
	if (!domain_) {
		Console::LogError("Failed to initialize Mono JIT", LogCategory::ScriptEngine);
		return;
	}

	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::LogError("Failed to find latest assembly DLL.", LogCategory::ScriptEngine);
		return;
	}

	currentDllPath_ = *latestDll;
	assembly_ = mono_domain_assembly_open(domain_, currentDllPath_.c_str());
	if (!assembly_) {
		Console::LogError("Failed to load CSharpLibrary.dll", LogCategory::ScriptEngine);
		return;
	}

	image_ = mono_assembly_get_image(assembly_);
	if (!image_) {
		Console::LogError("Failed to get image from assembly", LogCategory::ScriptEngine);
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
	AddEventInternalCalls();

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
	AddGizmoInternalCalls();
	AddWindowInternalCalls();
	AddAnimationInternalCalls();
	ComponentApplyFuncs::Initialize(image_);

	// データ同期用のC#メソッドを取得
	{
		// static class ComponentBatchManager
		receiveAllBatchesMethod_ = GetMethodFromCS("", "ComponentBatchManager", "ReceiveAllBatches", 2);

		// static class EntityComponentSystem
		getEcsGroupMethod_ = GetMethodFromCS("", "EntityComponentSystem", "GetECSGroup", 1);
		addEcsGroupMethod_ = GetMethodFromCS("", "EntityComponentSystem", "AddECSGroup", 1);
		clearEcsGroupMethod_ = GetMethodFromCS("", "EntityComponentSystem", "ClearECSGroup", 1);
		
		// class ECSGroup
		MonoClass* ecsGroupClass = mono_class_from_name(image_, "", "ECSGroup");
		if (ecsGroupClass) {
			getComponentCollectionField_ = MonoScriptEngineUtils::FindFieldRecursive(ecsGroupClass, "componentCollection");
			addEntityMethod_ = mono_class_get_method_from_name(ecsGroupClass, "AddEntity", 1);
		}

		// class Entity
		MonoClass* entityClass = mono_class_from_name(image_, "", "Entity");
		if (entityClass) {
			fetchInitialDataMethod_ = mono_class_get_method_from_name(entityClass, "FetchInitialData", 0);
		}

		// static class SceneManager
		MonoClass* sceneManagerClass = mono_class_from_name(image_, "", "SceneManager");
		if (sceneManagerClass) {
			sceneNameField_ = mono_class_get_field_from_name(sceneManagerClass, "sceneName_");
		}

		// AI
		updateAiIntentsMethod_ = GetMethodFromCS("", "AIUpdater", "UpdateIntents", 4);
		notifyEventCompletedMethod_ = GetMethodFromCS("", "BlackboardManager", "SetBool", 3);
	}
}

void MonoScriptEngine::HotReload() {
	MonoDomain* oldDomain = domain_;
	std::string oldDllPath = currentDllPath_;

	domain_ = CreateReloadDomain();
	mono_domain_set(domain_, true);

	if (domain_ != oldDomain) {
		Console::Log("Created new Mono domain for hot reload.", LogCategory::ScriptEngine);
	} else {
		Console::Log("Reusing existing Mono domain for hot reload.", LogCategory::ScriptEngine);
	}

	auto latestDll = FindLatestDll("./Packages/Scripts", "CSharpLibrary");
	if (!latestDll.has_value()) {
		Console::LogError("Failed to find latest assembly DLL.", LogCategory::ScriptEngine);
		mono_domain_set(oldDomain, true);
		mono_domain_unload(domain_);
		domain_ = oldDomain;
		return;
	}

	assembly_ = mono_domain_assembly_open(domain_, latestDll->c_str());
	if (!assembly_) {
		Console::LogError("Failed to load assembly in new domain", LogCategory::ScriptEngine);
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

	SetIsHotReloadRequest(true);

	Console::Log("Reloaded assembly successfully in new domain.", LogCategory::ScriptEngine);
}

void MonoScriptEngine::SetEcsPtr(EntityComponentSystem* _ecs) {
	pEcs_ = _ecs;
}

std::optional<std::string> MonoScriptEngine::FindLatestDll(const std::string& _dirPath, const std::string& _baseName) {
	std::regex pattern(_baseName + R"(.*\.dll)"); // プレフィックスが一致する全てのDLL
	std::optional<std::string> latestFile;
	std::filesystem::file_time_type latestTime;

	if (!std::filesystem::exists(_dirPath)) {
		return std::nullopt;
	}

	for (const auto& entry : std::filesystem::directory_iterator(_dirPath)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		std::string filename = entry.path().filename().string();
		if (!std::regex_match(filename, pattern)) {
			continue;
		}

		auto currentTime = std::filesystem::last_write_time(entry.path());

		if (!latestFile || currentTime > latestTime) {
			latestFile = entry.path().string();
			latestTime = currentTime;
		}
	}

	if (latestFile) {
		Console::Log("Latest DLL found: " + *latestFile, LogCategory::ScriptEngine);
	}

	return latestFile;
}

void MonoScriptEngine::ResetCS() {
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem", LogCategory::ScriptEngine);
		return;
	}

	MonoMethod* method = mono_class_get_method_from_name(monoClass, "DeleteEntityAll", 0);
	if (!method) {
		Console::LogError("Failed to find method: DeleteEntityAll", LogCategory::ScriptEngine);
		return;
	}

	MonoObject* exc = nullptr;
	mono_runtime_invoke(method, nullptr, nullptr, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
	}
}

MonoObject* MonoScriptEngine::GetEntityFromCS(const std::string& _ecsGroupName, int32_t _entityId) {
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem", LogCategory::ScriptEngine);
		return nullptr;
	}

	MonoMethod* method = mono_class_get_method_from_name(monoClass, "GetEntity", 2);
	if (!method) {
		Console::LogError("Failed to find method: GetEntity", LogCategory::ScriptEngine);
		return nullptr;
	}

	void* args[2];
	args[0] = mono_string_new(mono_domain_get(), _ecsGroupName.c_str());
	args[1] = &_entityId;

	MonoObject* exc = nullptr;
	MonoObject* result = mono_runtime_invoke(method, nullptr, args, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
		return nullptr;
	}

	return result;
}

MonoObject* MonoScriptEngine::GetMonoBehaviorFromCS(const std::string& _ecsGroupName, int32_t _entityId, const std::string& _behaviorName) {
	MonoClass* monoClass = mono_class_from_name(image_, "", "EntityComponentSystem");
	if (!monoClass) {
		Console::LogError("Failed to find class: EntityComponentSystem", LogCategory::ScriptEngine);
		return nullptr;
	}

	MonoMethod* method = mono_class_get_method_from_name(monoClass, "GetMonoBehavior", 3);
	if (!method) {
		Console::LogError("Failed to find method: GetMonoBehavior", LogCategory::ScriptEngine);
		return nullptr;
	}

	void* args[3];
	args[0] = mono_string_new(mono_domain_get(), _ecsGroupName.c_str());
	args[1] = &_entityId;
	args[2] = mono_string_new(mono_domain_get(), _behaviorName.c_str());

	MonoObject* exc = nullptr;
	MonoObject* result = mono_runtime_invoke(method, nullptr, args, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
		return nullptr;
	}

	return result;
}

GameEntity* MonoScriptEngine::GetOwnerEntity(MonoObject* _obj) {
	if (!_obj || !image_ || !pEcs_) return nullptr;

	MonoClass* klass = mono_object_get_class(_obj);
	if (!klass) return nullptr;

	// 'entity' プロパティを取得 (MonoScriptに定義されている)
	MonoProperty* entityProp = mono_class_get_property_from_name(klass, "entity");
	MonoObject* entityObj = nullptr;
	if (entityProp) {
		entityObj = mono_property_get_value(entityProp, _obj, nullptr, nullptr);
	} else {
		// プロパティがない場合はフィールドを探す (互換性のため)
		MonoClassField* entityField = mono_class_get_field_from_name(klass, "entity");
		if (entityField) {
			mono_field_get_value(_obj, entityField, &entityObj);
		}
	}

	if (!entityObj) {
		// _obj 自身が Entity クラスのインスタンスである可能性を考慮
		MonoClass* entityClass = mono_class_from_name(image_, "", "Entity");
		if (mono_class_is_assignable_from(entityClass, klass)) {
			entityObj = _obj;
		}
	}

	if (!entityObj) return nullptr;

	// Entity オブジェクトから 'entityId_' フィールドを取得
	MonoClass* entityKlass = mono_object_get_class(entityObj);
	MonoClassField* idField = mono_class_get_field_from_name(entityKlass, "entityId_");
	if (!idField) {
		idField = MonoScriptEngineUtils::FindFieldRecursive(entityKlass, "entityId_");
	}

	if (!idField) return nullptr;

	int32_t entityId = 0;
	mono_field_get_value(entityObj, idField, &entityId);

	// 全グループから検索
	for (auto& pair : pEcs_->GetECSGroups()) {
		GameEntity* entity = pair.second->GetEntityCollection()->GetEntity(entityId);
		if (entity) return entity;
	}

	return nullptr;
}

GameEntity* MonoScriptEngine::GetOwnerEntity(const Guid& _guid) {
	if (!pEcs_) return nullptr;

	for (auto& pair : pEcs_->GetECSGroups()) {
		GameEntity* entity = pair.second->GetEntityFromGuid(_guid);
		if (entity) return entity;
	}

	return nullptr;
}

std::string MonoScriptEngine::GetGroupNameByEntityGuid(const Guid& _guid) {
	if (!pEcs_) return "";

	for (auto& pair : pEcs_->GetECSGroups()) {
		if (pair.second->GetEntityFromGuid(_guid)) {
			return pair.first;
		}
	}

	return "";
}

MonoMethod* MonoScriptEngine::GetMethodFromCS(const std::string& _namespace, const std::string& _className, const std::string& _methodName, int _argsCount) {
	/// MonoClassを取得
	MonoClass* monoClass = mono_class_from_name(image_, _namespace.c_str(), _className.c_str());
	if (!monoClass) {
		Console::LogError("Failed to find class: " + (_namespace.empty() ? "" : _namespace + ".") + _className, LogCategory::ScriptEngine);
		return nullptr;
	}

	for (MonoClass* current = monoClass; current != nullptr; current = mono_class_get_parent(current)) {
		MonoMethod* method = mono_class_get_method_from_name(current, _methodName.c_str(), _argsCount);
		if (method) {
			return method;
		}
	}

	Console::LogError("Failed to find method: " + (_namespace.empty() ? "" : _namespace + ".") + _className + "::" + _methodName, LogCategory::ScriptEngine);
	return nullptr;
}

MonoDomain* MonoScriptEngine::CreateReloadDomain() {
	std::string domainName = "ReloadedDomain_" + std::to_string(++domainReloadCounter_);

	MonoDomain* domain = mono_domain_create_appdomain((char*)domainName.c_str(), nullptr);
	if (!domain) {
		Console::LogError("Failed to create Mono domain for hot reload: " + domainName, LogCategory::ScriptEngine);
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

std::vector<MonoScriptEngine::NodeClassInfo> MonoScriptEngine::GetBehaviorNodeClasses() {
	std::vector<NodeClassInfo> nodeClasses;
	if (!image_) return nodeClasses;

	MonoClass* baseClass = mono_class_from_name(image_, "", "BehaviorNode");
	if (!baseClass) {
		Console::LogError("BehaviorNode class not found in C# assembly.", LogCategory::ScriptEngine);
		return nodeClasses;
	}

	MonoClass* decoratorAttrClass = mono_class_from_name(image_, "", "DecoratorAttribute");

	const MonoTableInfo* tableInfo = mono_image_get_table_info(image_, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(tableInfo);

	for (int i = 0; i < rows; i++) {
		MonoClass* klass = mono_class_get(image_, (i + 1) | MONO_TOKEN_TYPE_DEF);
		if (!klass) continue;

		// 抽象クラスやインターフェースは除外
		uint32_t flags = mono_class_get_flags(klass);
		if (flags & (0x00000080 /* TYPE_ATTRIBUTE_ABSTRACT */ | 0x00000020 /* TYPE_ATTRIBUTE_INTERFACE */)) {
			continue;
		}

		if (mono_class_is_subclass_of(klass, baseClass, false)) {
			const char* className = mono_class_get_name(klass);
			const char* nameSpace = mono_class_get_namespace(klass);
			
			NodeClassInfo info;
			info.fullName = (nameSpace && strlen(nameSpace) > 0) 
				? std::string(nameSpace) + "." + className 
				: std::string(className);
			
			// Decorator属性のチェック
			if (decoratorAttrClass) {
				MonoCustomAttrInfo* attrs = mono_custom_attrs_from_class(klass);
				if (attrs) {
					if (mono_custom_attrs_has_attr(attrs, decoratorAttrClass)) {
						info.isDecorator = true;
					}
					mono_custom_attrs_free(attrs);
				}
			}

			nodeClasses.push_back(info);
		}
	}

	return nodeClasses;
}

std::vector<MonoScriptEngine::FieldInfo> MonoScriptEngine::GetClassFields(const std::string& className) {
	std::vector<FieldInfo> fields;
	if (!image_) return fields;

	MonoClass* klass = mono_class_from_name(image_, "", className.c_str());
	if (!klass) {
		// 名前空間ありの場合
		size_t dotPos = className.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ns = className.substr(0, dotPos);
			std::string name = className.substr(dotPos + 1);
			klass = mono_class_from_name(image_, ns.c_str(), name.c_str());
		}
	}

	if (!klass) return fields;

	void* iter = nullptr;
	MonoClassField* field;
	while ((field = mono_class_get_fields(klass, &iter))) {
		uint32_t flags = mono_field_get_flags(field);
		if (!(flags & 0x0006 /* FIELD_ATTRIBUTE_PUBLIC */)) continue;

		FieldInfo info;
		info.name = mono_field_get_name(field);
		
		MonoType* type = mono_field_get_type(field);
		char* typeName = mono_type_get_name(type);
		info.typeName = typeName;
		mono_free(typeName);

		// 属性のチェック (BlackboardKeyAttribute)
		MonoCustomAttrInfo* attrs = mono_custom_attrs_from_field(klass, field);
		if (attrs) {
			MonoClass* attrClass = mono_class_from_name(image_, "", "BlackboardKeyAttribute");
			if (attrClass && mono_custom_attrs_has_attr(attrs, attrClass)) {
				info.isBBKey = true;
			}
			mono_custom_attrs_free(attrs);
		}

		fields.push_back(info);
	}

	return fields;
}

std::vector<MonoScriptEngine::NodeClassInfo> MonoScriptEngine::GetBehaviorModuleClasses() {
	std::vector<NodeClassInfo> moduleClasses;
	if (!image_) return moduleClasses;

	MonoClass* decoratorBase = mono_class_from_name(image_, "", "BehaviorDecorator");
	MonoClass* serviceBase = mono_class_from_name(image_, "", "BehaviorService");
	
	const MonoTableInfo* tableInfo = mono_image_get_table_info(image_, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(tableInfo);

	for (int i = 0; i < rows; i++) {
		MonoClass* klass = mono_class_get(image_, (i + 1) | MONO_TOKEN_TYPE_DEF);
		if (!klass) continue;

		uint32_t flags = mono_class_get_flags(klass);
		if (flags & (0x00000080 | 0x00000020)) continue;

		bool isDecorator = decoratorBase && mono_class_is_subclass_of(klass, decoratorBase, false);
		bool isService = serviceBase && mono_class_is_subclass_of(klass, serviceBase, false);

		if (isDecorator || isService) {
			const char* className = mono_class_get_name(klass);
			const char* nameSpace = mono_class_get_namespace(klass);
			
			NodeClassInfo info;
			info.fullName = (nameSpace && strlen(nameSpace) > 0) 
				? std::string(nameSpace) + "." + className 
				: std::string(className);
			info.isDecorator = isDecorator; // true: Decorator, false: Service
			moduleClasses.push_back(info);
		}
	}
	return moduleClasses;
}

void MonoScriptEngine::UpdateAiIntents(void* data, int count, float deltaTime, const std::string& groupName) {
	if (!updateAiIntentsMethod_) {
		Console::LogWarning("AIUpdater.UpdateIntents method not found in C#.", LogCategory::ScriptEngine);
		return;
	}

	void* args[4];
	args[0] = data;
	args[1] = &count;
	args[2] = &deltaTime;
	args[3] = mono_string_new(domain_, groupName.c_str());

	MonoObject* exc = nullptr;
	mono_runtime_invoke(updateAiIntentsMethod_, nullptr, args, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown in AIUpdater.UpdateIntents: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
	}
}

void MonoScriptEngine::NotifyEventCompleted(int32_t entityId, const std::string& eventName) {
	if (!notifyEventCompletedMethod_) {
		return;
	}

	std::string key = "EventComplete_" + eventName;
	MonoString* keyStr = mono_string_new(domain_, key.c_str());
	bool value = true;

	void* args[3];
	args[0] = &entityId;
	args[1] = keyStr;
	args[2] = &value;

	MonoObject* exc = nullptr;
	mono_runtime_invoke(notifyEventCompletedMethod_, nullptr, args, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown in BlackboardManager.SetBool: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
	}
}

void MonoScriptEngine::ClearECSGroup(const std::string& _name) {
	if (!clearEcsGroupMethod_) {
		return;
	}

	void* args[1];
	args[0] = mono_string_new(domain_, _name.c_str());

	MonoObject* exc = nullptr;
	mono_runtime_invoke(clearEcsGroupMethod_, nullptr, args, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown in EntityComponentSystem.ClearECSGroup: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
	}
}

void MonoScriptEngine::SyncInitialComponentsToCS(ECSGroup* _ecsGroup) {
	if (!_ecsGroup) {
		return;
	}

	const std::string& ecsGroupName = _ecsGroup->GetGroupName();

	if (!addEcsGroupMethod_ || !getComponentCollectionField_ || !receiveAllBatchesMethod_) {
		Console::LogError("One or more methods for SyncInitialComponentsToCS are not found.", LogCategory::ScriptEngine);
		return;
	}

	MonoObject* exc = nullptr;

	// C#側のシーン名を更新
	if (sceneNameField_) {
		MonoString* nameStr = mono_string_new(domain_, ecsGroupName.c_str());
		MonoClass* parentClass = mono_field_get_parent(sceneNameField_);
		MonoVTable* vtable = mono_class_vtable(domain_, parentClass);
		mono_field_static_set_value(vtable, sceneNameField_, nameStr);
	}

	void* getGroupArgs[1];
	getGroupArgs[0] = mono_string_new(domain_, ecsGroupName.c_str());
	MonoObject* ecsGroupObject = mono_runtime_invoke(addEcsGroupMethod_, nullptr, getGroupArgs, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception in AddECSGroup: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
		return;
	}
	if (!ecsGroupObject) {
		Console::LogError("C# ECSGroup object is null for group: " + ecsGroupName, LogCategory::ScriptEngine);
		return;
	}

	if (addEntityMethod_) {
		for (const auto& entity : _ecsGroup->GetEntities()) {
			int32_t id = entity->GetId();
			void* addArgs[1];
			addArgs[0] = &id;
			mono_runtime_invoke(addEntityMethod_, ecsGroupObject, addArgs, &exc);
			if (exc) {
				char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
				Console::LogError(std::string("Exception in AddEntity: ") + err, LogCategory::ScriptEngine);
				mono_free(err);
				exc = nullptr;
				continue;
			}
		}
	}

	MonoObject* collectionObject = mono_field_get_value_object(domain_, getComponentCollectionField_, ecsGroupObject);
	if (!collectionObject) {
		Console::LogError("C# ComponentCollection object is null for group: " + ecsGroupName, LogCategory::ScriptEngine);
		return;
	}

	exc = nullptr;
	void* receiveArgs[2];
	receiveArgs[0] = collectionObject;
	receiveArgs[1] = mono_string_new(domain_, ecsGroupName.c_str());
	mono_runtime_invoke(receiveAllBatchesMethod_, nullptr, receiveArgs, &exc);
	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception in ReceiveAllBatches: ") + err, LogCategory::ScriptEngine);
		mono_free(err);
		return;
	}

	Console::Log("Successfully synced initial components to C# for group: " + ecsGroupName, LogCategory::ScriptEngine);
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

MonoClassField* ONEngine::MonoScriptEngineUtils::FindFieldRecursive(MonoClass* _class, const char* _name) {
	while(_class) {
		MonoClassField* field = mono_class_get_field_from_name(_class, _name);
		if(field) {
			return field;
		}
		_class = mono_class_get_parent(_class);
	}
	return nullptr;
}
