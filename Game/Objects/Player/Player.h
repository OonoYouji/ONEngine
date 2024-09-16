#pragma once

#include <GameObjectManager.h>

class Player : public BaseGameObject {
public:

	Player() { CreateTag(this); }
	~Player() {}

	void Initialize() override;
	void Update() override {}

private:

};