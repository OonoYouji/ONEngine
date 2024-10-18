#pragma once

#include "GameObjectManager/GameObjectManager.h"

class GameResult : public BaseGameObject {
public:

	GameResult();
	~GameResult();

	void Initialize() override;
	void Update()     override;

private:

};
