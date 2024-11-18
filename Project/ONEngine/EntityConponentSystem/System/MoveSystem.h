#pragma once

#include "ISystem/ISystem.h"


class MoveSystem : public ISystem {
public:

	MoveSystem() {}
	~MoveSystem() {}

	void Initialize() override;
	void Update()     override;

};