#include "ScriptUpdateSystem.h"

/// std
#include <list>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

ScriptUpdateSystem::ScriptUpdateSystem() {}
ScriptUpdateSystem::~ScriptUpdateSystem() {}

void ScriptUpdateSystem::Update(EntityComponentSystem* _entityComponentSystem) {

	std::list<Script*> scripts;
	for (auto& entity : _entityComponentSystem->GetEntities()) {
		Script* script = entity->GetComponent<Script>();
		if (script) {
			scripts.push_back(script);
		}
	}

	MonoObject* exception = nullptr;
	for (auto& script : scripts) {
		if (script->updateMethod_) {
			mono_runtime_invoke(script->updateMethod_, script->instance_, nullptr, &exception);
		}

		if (exception) {
			Console::Log("Exception occurred in Initialize()");
			// さらに mono_print_unhandled_exception などで例外を表示可能
		}
	}
}
