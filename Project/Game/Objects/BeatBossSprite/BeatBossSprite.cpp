#define NOMINMAX
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
	spriteRenderer_->SetTexture("telop.png");
	spriteRenderer_->SetUVSize({ 2048.0f, 128.0f });

	pTransform_->position = { 0.0f, -0.55f, 0.0f };
	pTransform_->scale    = { 2.5f, 0.25f, 1.0f };

	spritePosition_ = {};
	spriteColor_ = Vec4::kWhite;

	time_ = 0.0f;
}

void BeatBossSprite::Update() {

	time_ += Time::DeltaTime();

	spritePosition_.x += 120.0f * Time::DeltaTime();

	float rgbScalerValue = std::sin(time_ * 2.0f) * 0.25f + 0.75f;

	spriteColor_ = {
		1.0f * rgbScalerValue,
		1.0f * rgbScalerValue,
		1.0f * rgbScalerValue,
		1.0f * rgbScalerValue
	};

	spriteRenderer_->SetUVPosition(spritePosition_);
	spriteRenderer_->SetColor(spriteColor_);
}
