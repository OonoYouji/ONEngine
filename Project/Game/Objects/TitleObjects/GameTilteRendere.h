#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameTitleRenderer final : public BaseGameObject {
public:

	GameTitleRenderer() { CreateTag(this); }
	~GameTitleRenderer() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:


};