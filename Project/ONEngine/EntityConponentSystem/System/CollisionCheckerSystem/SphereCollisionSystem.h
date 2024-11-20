#pragma once

#include "../ISystem/ISystem.h"


class SphereCollisionSystem : public ISystem {
public:

	SphereCollisionSystem() {}
	~SphereCollisionSystem() {}

	void Update(size_t _entity, class ECSManager* _ecsManager) override;
};