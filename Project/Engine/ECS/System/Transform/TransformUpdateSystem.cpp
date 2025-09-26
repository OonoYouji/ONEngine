#include "TransformUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

void TransformUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	ComponentArray<Transform>* transformArray = _ecs->GetComponentArray<Transform>();
	if (!transformArray) {
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

void TransformUpdateSystem::RuntimeUpdate(ECSGroup*) {}
