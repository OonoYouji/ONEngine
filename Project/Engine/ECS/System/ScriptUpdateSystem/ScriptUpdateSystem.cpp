#include "ScriptUpdateSystem.h"

/// std
#include <list>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

ScriptUpdateSystem::ScriptUpdateSystem() {}
ScriptUpdateSystem::~ScriptUpdateSystem() {}

void ScriptUpdateSystem::Update([[maybe_unused]] EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {

	std::list<Script*> scripts;
	for (auto& entity : _entities) {
		Script* script = entity->GetComponent<Script>();
		if (script) {
			scripts.push_back(script);
		}
	}

	for (auto& scriptComp : scripts) {
		for (auto& script : scriptComp->scriptDataList_) {

			/// スクリプトが有効でなければスキップ
			if (!script.enable) {
				continue;
			}

			/// クラスが同じかチェック
			MonoClass* instanceClass = mono_object_get_class(script.instance);
			MonoClass* methodClass = mono_method_get_class(script.updateMethod);
			if (instanceClass != methodClass) {
				Console::LogError("ScriptUpdateSystem: Instance class does not match method class for script: " + script.scriptName);
				continue;
			}

			if (script.updateMethod && script.instance) {
				/// 例外をキャッチするためのポインタ
				MonoObject* exc = nullptr;

				/// updateメソッドを呼び出す
				mono_runtime_invoke(script.updateMethod, script.instance, nullptr, &exc);

				/// 例外が発生したらここで出力
				if (exc) {
					MonoString* monoStr = mono_object_to_string(exc, nullptr);
					if (monoStr) {
						char* message = mono_string_to_utf8(monoStr);
						Console::LogError(std::string("Mono Exception: ") + message);
						mono_free(message);
					} else {
						Console::LogError("Mono Exception occurred, but message is null.");
					}
				}

			}

		}

	}
}
