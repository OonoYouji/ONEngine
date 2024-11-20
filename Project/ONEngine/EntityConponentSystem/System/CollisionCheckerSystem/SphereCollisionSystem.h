#pragma once

#include "../ISystem/ISystem.h"


class SphereCollisionSystem : public ISystem {
public:

	SphereCollisionSystem() {}
	~SphereCollisionSystem() {}

	void Update(size_t _entity, class ECSManager* _ecsManager) override;

	bool CheckCollision(
		size_t _otherEntity, 
		ECSManager* _ecsManager,
		struct PositionComponent* _thisEntityPositon,
		struct SphereColliderComponent* _thisEntitySphereCollder
	);
};