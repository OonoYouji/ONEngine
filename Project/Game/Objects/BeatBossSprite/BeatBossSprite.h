#pragma once

#include "GameObjectManager/GameObjectManager.h"

class BeatBossSprite : public BaseGameObject {
public:

	BeatBossSprite();
	~BeatBossSprite();

	void Initialize() override;
	void Update()     override;

	void SetIsFadeOut(bool _isFadeOut);

private:

	class SpriteRenderer* spriteRenderer_ = nullptr;
	Vec2 spritePosition_;
	Vec4 spriteColor_;

	float time_;
	bool  isFadeOut_;
};
