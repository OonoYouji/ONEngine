#include "BeatBossSprite.h"

/// engine
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


BeatBossSprite::BeatBossSprite() {
	CreateTag(this);
}

BeatBossSprite::~BeatBossSprite() {}

void BeatBossSprite::Initialize() {

	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("uvChecker");

}

void BeatBossSprite::Update() {
	spritePosition_.x += 120.0f * Time::DeltaTime();
	spriteRenderer_->SetUVPosition(spritePosition_);
	//spriteRenderer_->;
}
