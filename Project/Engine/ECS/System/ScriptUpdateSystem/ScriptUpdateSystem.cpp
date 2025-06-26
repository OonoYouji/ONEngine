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

	for (auto& script : scripts) {
		if (script->updateMethod_ && script->instance_) {

			MonoObject* exc = nullptr;
			mono_runtime_invoke(script->updateMethod_, script->instance_, nullptr, &exc);

			if (exc) {
				MonoString* excMessage = mono_object_to_string(exc, nullptr);
				const char* message = mono_string_to_utf8(excMessage);
				Console::Log("Script Update Error: " + std::string(message));
				mono_free((void*)message);
			}
		}
	}
}
