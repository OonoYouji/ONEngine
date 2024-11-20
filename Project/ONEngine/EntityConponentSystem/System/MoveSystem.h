#pragma once

#include "ISystem/ISystem.h"


class MoveSystem : public ISystem {
public:

	MoveSystem() {}
	~MoveSystem() {}

	void Update(size_t _entity, class ECSManager* _ecsManager) override;

};