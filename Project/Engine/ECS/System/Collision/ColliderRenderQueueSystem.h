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

	void Update(class EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) override;

	void UpdateSphereCollider(const std::vector<class IEntity*>& _entities);
	void UpdateBoxCollider(const std::vector<class IEntity*>& _entities);

};

