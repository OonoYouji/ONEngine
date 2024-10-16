#pragma once

/// base class
#include "GameObjectManager/GameObjectManager.h"


enum BOSS_ANIMATION_ORDER {
	ENTRY, /// 出現
};


class BossAnimation : BaseGameObject {
public:

	BossAnimation();
	~BossAnimation();

	void Initialize() override;
	void Update()     override;

private:

	

};

