#include "ScriptUpdateSystem.h"

/// std
#include <list>

/// external
#include <mono/metadata/mono-gc.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/Script/MonoScriptEngine.h"

ScriptUpdateSystem::ScriptUpdateSystem(ECSGroup* _ecs) {
	pImage_ = GetMonoScriptEnginePtr()->Image();
	MakeScriptMethod(pImage_, _ecs->GetGroupName());
}

ScriptUpdateSystem::~ScriptUpdateSystem() {}

void ScriptUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	MonoScriptEngine* monoEngine = GetMonoScriptEnginePtr();
	if (!monoEngine) {
		return;
	}

	if (monoEngine->GetIsHotReloadRequest()) {
		/// C#側のECSGroupを取得、更新関数を呼ぶ
		ComponentArray<Script>* scriptArray = _ecs->GetComponentArray<Script>();
		if (!scriptArray || scriptArray->GetUsedComponents().empty()) {
			return;
		}

		for(auto& script : scriptArray->GetUsedComponents()) {
			script->SetIsAdded(false);
		}


		MakeScriptMethod(pImage_, _ecs->GetGroupName());
	}
}

void ScriptUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
#ifdef DEBUG_MODE
	{	/// debug monoのヒープの状態を出力
		size_t heapSize = mono_gc_get_heap_size();
		size_t usedSize = mono_gc_get_used_size();

		Console::LogInfo("[mono] GC Heap Size : " + std::to_string(heapSize / 1024) + "KB");
		Console::LogInfo("[mono] GC Used Size : " + std::to_string(usedSize / 1024) + "KB");
	}
#endif // DEBUG_MODE

	/// C#側のECSGroupを取得、更新関数を呼ぶ
	ComponentArray<Script>* scriptArray = _ecs->GetComponentArray<Script>();
	if (!scriptArray || scriptArray->GetUsedComponents().empty()) {
		return;
	}

	for (auto& script : scriptArray->GetUsedComponents()) {
		/// C#に対してエンティティが追加済みなら無視
		if (script->GetIsAdded()) {
			continue;
		}

		/// 追加した
		script->SetIsAdded(true);
		MonoObject* ecsGroupObj = mono_gchandle_get_target(gcHandle_);
		if (!ecsGroupObj) {
			Console::LogError("Failed to get target from gcHandle_");
			return;
		}

		{	/// Entityの追加関数を呼び出す
			void* args[1];
			int32_t entityId = script->GetOwner()->GetId();
			args[0] = &entityId;

			MonoObject* exc = nullptr;
			mono_runtime_invoke(addEntityMethod_, ecsGroupObj, args, &exc);

			if (exc) {
				/// 例外の処理
				char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
				Console::LogError(std::string("Exception thrown in UpdateEntities: ") + err);
				mono_free(err);
			}
		}

		{	/// Scriptの追加関数を呼び出す

			void* args[2];
			int32_t entityId = script->GetOwner()->GetId();

			for (auto& data : script->GetScriptDataList()) {
				/// スクリプト名からMonoObjectを生成する
				MonoClass* behaviorClass = mono_class_from_name(pImage_, "", data.scriptName.c_str());
				if (!behaviorClass) {
					Console::LogError("Failed to find MonoBehavior class");
					continue;
				}

				/// インスタンスを生成
				MonoObject* scriptInstance = mono_object_new(mono_domain_get(), behaviorClass);
				mono_runtime_object_init(scriptInstance); /// クラスの初期化、コンストラクタをイメージ
				if (!script) {
					continue;
				}

				args[0] = &entityId;
				args[1] = scriptInstance;

				MonoObject* exc = nullptr;
				mono_runtime_invoke(addScriptMethod_, ecsGroupObj, args, &exc);

				if (exc) {
					/// 例外の処理
					char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
					Console::LogError(std::string("Exception thrown in AddScript: ") + err);
					mono_free(err);
				}

			}

		}
	}


	/// 関数呼び出しの条件
	if (gcHandle_ != 0) {
		if (updateEntitiesMethod_) {

			/// 更新関数を呼び出す
			MonoObject* ecsGroupObj = mono_gchandle_get_target(gcHandle_);
			if (!ecsGroupObj) {
				Console::LogError("Failed to get target from gcHandle_");
				return;
			}

			MonoObject* exc = nullptr;
			mono_runtime_invoke(updateEntitiesMethod_, ecsGroupObj, nullptr, &exc);

			if (exc) {
				/// 例外の処理
				char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
				Console::LogError(std::string("Exception thrown in UpdateEntities: ") + err);
				mono_free(err);
			}

		}
	}

}


void ScriptUpdateSystem::MakeScriptMethod(MonoImage* _image, const std::string& _ecsGroupName) {

	/// --------------------------------------------------------------------------------
	/// EntityComponentSystemの関数から新規にグループを追加する
	/// --------------------------------------------------------------------------------

	MonoClass* ecsClass = mono_class_from_name(_image, "", "EntityComponentSystem");
	if (!ecsClass) {
		Console::LogError("Failed to find class: EntityComponentSystem");
		return;
	}

	/// EntityComponentSystemのAddECSGroup関数を取得
	MonoMethod* addGroupMethod = MonoScriptEngineUtils::FindMethodInClassOrParents(ecsClass, "AddECSGroup", 1);

	/// 関数の引数
	void* args[1];
	args[0] = mono_string_new(mono_domain_get(), _ecsGroupName.c_str());; /// ECSのGroup名

	/// 関数を呼び出す
	MonoObject* exc = nullptr;
	MonoObject* ecsGroup = mono_runtime_invoke(addGroupMethod, nullptr, args, &exc);

	if (exc) {
		char* err = mono_string_to_utf8(mono_object_to_string(exc, nullptr));
		Console::LogError(std::string("Exception thrown: ") + err);
		mono_free(err);
	}


	/// --------------------------------------------------------------------------------
	/// C#側のECSGroupクラスを取得
	/// --------------------------------------------------------------------------------

	/// C#側のクラスを取得
	monoClass_ = mono_object_get_class(ecsGroup);
	if (!monoClass_) {
		Console::LogError("Failed to find class: ECSGroup");
		return;
	}

	gcHandle_ = mono_gchandle_new(ecsGroup, false);

	/// 呼び出し対象の関数を取得
	updateEntitiesMethod_ = mono_class_get_method_from_name(monoClass_, "UpdateEntities", 0);
	addEntityMethod_ = mono_class_get_method_from_name(monoClass_, "AddEntity", 1);
	addScriptMethod_ = mono_class_get_method_from_name(monoClass_, "AddScript", 2);

}
