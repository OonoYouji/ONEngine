#pragma once

#include "GameObjectManager/GameObjectManager.h"

class BossParts : BaseGameObject {
public:

	BossParts();
	~BossParts();

	void Initialize() override;
	void Update()     override;

private:

};
