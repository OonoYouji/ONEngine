#pragma once

#include "../ISystem/ISystem.h"

class MovementSystem : public ISystem {
public:

	MovementSystem() {}
	~MovementSystem() {}

	void Update(size_t _entity, class ECSManager* _ecsManager) override;
};