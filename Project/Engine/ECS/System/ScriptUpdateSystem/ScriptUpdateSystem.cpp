#include "ScriptUpdateSystem.h"

/// std
#include <list>
#include <chrono>

/// external
#include <mono/metadata/mono-gc.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/Script/MonoScriptEngine.h"

ScriptUpdateSystem::ScriptUpdateSystem(ECSGroup* _ecs) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();
	MakeScriptMethod(monoEngine->Image(), _ecs->GetGroupName());
}

ScriptUpdateSystem::~ScriptUpdateSystem() {
	ReleaseGCHandle();
}

void ScriptUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();
	if (!monoEngine) {
		return;
	}

	if (monoEngine->GetIsHotReloadRequest()) {
		/// C#側のECSGroupを取得、更新関数を呼ぶ
		ComponentArray<Script>* scriptArray = _ecs->GetComponentArray<Script>();
		if (!scriptArray || scriptArray->GetUsedComponents().empty()) {
			return;
		}

		for (auto& script : scriptArray->GetUsedComponents()) {
			script->SetIsAdded(false);
			for(auto& data : script->GetScriptDataList()) {
				data.isAdded = false;
			}
		}

		ReleaseGCHandle();
		MakeScriptMethod(monoEngine->Image(), _ecs->GetGroupName());
	}
}

void ScriptUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
#ifdef DEBUG_MODE
	/// この関数の処理にかかっている時間を計算する
	auto startTime = std::chrono::high_resolution_clock::now();

	{	/// debug monoのヒープの状態を出力
		size_t heapSize = mono_gc_get_heap_size();
		size_t usedSize = mono_gc_get_used_size();

		Console::LogInfo("[mono] GC Heap Size : " + std::to_string(heapSize / 1024) + "KB");
		Console::LogInfo("[mono] GC Used Size : " + std::to_string(usedSize / 1024) + "KB");
	}
#endif // DEBUG_MODE

	/// C#側に未追加にエンティティとコンポーネントを追加する
	AddEntityAndComponent(_ecs);

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


#ifdef DEBUG_MODE
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
	Console::LogInfo("[ScriptUpdateSystem] RuntimeUpdate took " + std::to_string(duration) + " microseconds");
#endif // DEBUG_MODE
}

void ScriptUpdateSystem::AddEntityAndComponent(ECSGroup* _ecsGroup) {
	MonoScriptEngine* monoEngine = MonoScriptEngine::GetInstance();

	/// C#側のECSGroupを取得、更新関数を呼ぶ
	ComponentArray<Script>* scriptArray = _ecsGroup->GetComponentArray<Script>();
	if (!scriptArray || scriptArray->GetUsedComponents().empty()) {
		return;
	}

	for (auto& script : scriptArray->GetUsedComponents()) {

		/// スクリプトが有効でない場合はスキップ
		MonoObject* ecsGroupObj = mono_gchandle_get_target(gcHandle_);
		if (!ecsGroupObj) {
			Console::LogError("Failed to get target from gcHandle_");
			return;
		}


		/// Entityの追加関数を呼び出す
		if (!script->GetIsAdded()) {
			script->SetIsAdded(true);

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

		Variables* vars = script->GetOwner()->GetComponent<Variables>();

		for (auto& data : script->GetScriptDataList()) {

			/// すでに追加済みなら処理しない
			if (data.isAdded) {
				continue;
			}
			data.isAdded = true;

			/// スクリプト名からMonoObjectを生成する
			MonoClass* behaviorClass = mono_class_from_name(monoEngine->Image(), "", data.scriptName.c_str());
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

			void* args[2];
			int32_t entityId = script->GetOwner()->GetId();
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

			/// variablesの設定
			if (vars) {
				vars->SetScriptVariables(data.scriptName);
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

void ScriptUpdateSystem::ReleaseGCHandle() {
	if(gcHandle_ != 0) {
		mono_gchandle_free(gcHandle_);
		gcHandle_ = 0;
	}
}


/// ///////////////////////////////////////////////
/// デバッグ用のスクリプト更新システム
/// ///////////////////////////////////////////////

DebugScriptUpdateSystem::DebugScriptUpdateSystem(ECSGroup* _ecs) 
	: ScriptUpdateSystem(_ecs) {}
DebugScriptUpdateSystem::~DebugScriptUpdateSystem() {}

void DebugScriptUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	/// デバッグのスクリプトは常時更新したいのでRuntimeの更新も呼び出すようにする
	ScriptUpdateSystem::OutsideOfRuntimeUpdate(_ecs);
	ScriptUpdateSystem::RuntimeUpdate(_ecs);
}

void DebugScriptUpdateSystem::RuntimeUpdate(ECSGroup*) {}
