#pragma once

#include "GameObjectManager/GameObjectManager.h"

class BeatBossSprite : public BaseGameObject {
public:

	BeatBossSprite();
	~BeatBossSprite();

	void Initialize() override;
	void Update()     override;

private:

};
