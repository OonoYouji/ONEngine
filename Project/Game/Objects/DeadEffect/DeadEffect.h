#pragma once

#include "GameObjectManager/GameObjectManager.h"

class DeadEffect : public BaseGameObject {
public:

	DeadEffect();
	~DeadEffect();

	void Initialize() override;
	void Update()     override;

private:

};
