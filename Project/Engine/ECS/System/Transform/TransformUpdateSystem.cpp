#include "TransformUpdateSystem.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

/**
 * @brief エディタ非実行時のワールド行列の再計算および階層伝搬更新を行います。
 */
void TransformUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	Update(_ecs);
}

/**
 * @brief ランタイム実行時の全アクティブエンティティのワールド行列の再計算および親子関係の更新を実行します。
 */
void TransformUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {
	Update(_ecs);
}


/**
 * @brief 指定されたECSグループ内の全エンティティの Transform のワールド行列を更新する共通処理です。
 */
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
