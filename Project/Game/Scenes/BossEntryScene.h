#pragma once

#include "Manager/BaseScene.h"

class BossEntryScene final : public BaseScene {
public:
	BossEntryScene() {}
	~BossEntryScene() {}

	void Initialize() override;
	void Update()     override;

private:

	class BossAnimation*              bossAnimation_          = nullptr;
	class CameraStateBossEntryToGame* cameraState_            = nullptr;
	class BeatBossSprite*             beatBossSpriteArray_[2] = {};
};