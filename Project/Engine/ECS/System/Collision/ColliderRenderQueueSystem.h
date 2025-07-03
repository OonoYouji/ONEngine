#pragma once

/// engine
#include "../Interface/ECSISystem.h"

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

	void Update(class EntityComponentSystem* _ecs) override;

	void UpdateSphereCollider(class EntityComponentSystem* _ecs);
	void UpdateBoxCollider(class EntityComponentSystem* _ecs);

};

