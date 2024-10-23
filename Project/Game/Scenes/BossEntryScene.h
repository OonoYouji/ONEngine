#pragma once

#include <memory>

#include "ComponentManager/AudioSource/AudioSource.h"


/// base class
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

	std::unique_ptr<AudioSource> bgm_;
};