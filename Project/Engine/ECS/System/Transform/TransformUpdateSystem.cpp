#include "TransformUpdateSystem.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

void TransformUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	if (!DebugConfig::isDebugging) {
		Update(_ecs);
	}

}

void TransformUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
	if (DebugConfig::isDebugging) {
		Update(_ecs);
	}
}


void TransformUpdateSystem::Update(ECSGroup* _ecs) {
	/// ----- Transformの行列を更新する ----- ///

	ComponentArray<Transform>* transformArray = _ecs->GetComponentArray<Transform>();
	if (!transformArray || transformArray->GetUsedComponents().empty()) {
		Console::LogError("TransformUpdateSystem::OutsideOfRuntimeUpdate: Transform component array is null");
		return;
	}

	for (auto& transform : transformArray->GetUsedComponents()) {
		/// 行列を更新しない条件
		if (!transform || !transform->enable) {
			continue;
		}

		/// 行列を更新
		if (GameEntity* owner = transform->GetOwner()) {
			owner->UpdateTransform();
		}
	}

}