#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"

/// ///////////////////////////////////////////////////////
/// コライダーの可視化を行うシステム
/// ///////////////////////////////////////////////////////
class ColliderRenderQueueSystem : public ECSISystem {
public:
	/// =========================================================
	/// public : methods
	/// =========================================================

	ColliderRenderQueueSystem();
	~ColliderRenderQueueSystem() override = default;

	void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
	void RuntimeUpdate(class ECSGroup*) override {};

	/// @brief SphereのデータからGizmoを更新する
	void UpdateSphereCollider(ComponentArray<SphereCollider>* _sphereColliderArray);

	/// @brief BoxのデータからGizmoを更新する
	void UpdateBoxCollider(ComponentArray<BoxCollider>* _boxColliderArray);

};

