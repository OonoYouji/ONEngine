#pragma once

#include "GameObjectManager/GameObjectManager.h"

class Reticle : BaseGameObject {
public:

	Reticle();
	~Reticle();

	void Initialize() override;
	void Update()     override;

private:

};
