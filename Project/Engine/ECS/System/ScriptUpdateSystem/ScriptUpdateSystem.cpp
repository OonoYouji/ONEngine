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

	/// 更新関数の呼びだし
	for (auto& scriptComp : scripts) {
		/// Updateメソッドの呼び出し
		scriptComp->CallUpdateMethodAll();
	}

	/// 行列の更新
	for (auto& script : scripts) {
		IEntity* entity = script->GetOwner();
		if (entity) {
			entity->UpdateTransform();
		}
	}
}
