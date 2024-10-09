#pragma once

#include "GameObjectManager/GameObjectManager.h"

class AnchorPointObject : BaseGameObject {
public:

	AnchorPointObject();
	~AnchorPointObject();

	void Initialize() override;
	void Update()     override;

private:

};
