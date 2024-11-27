#include "ResultGameClearTimeRenderer.h"

#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "SecondRenderer/SecondRenderer.h"
#include "MinuteRenderer/MinuteRenderer.h"

#include "Scenes/Scene_Result.h"

ResultGameClearTimeRenderer::ResultGameClearTimeRenderer(float _time)
	: time_(_time) {
	CreateTag(this);
}

ResultGameClearTimeRenderer::~ResultGameClearTimeRenderer() {}

void ResultGameClearTimeRenderer::Initialize() {

	auto renderer = AddComponent<SpriteRenderer>();
	renderer->SetTexture("colon.png");

	float second = std::fmod(time_, 60.0f);
	float minute = time_ / 60.0f;

	secondRenderer_ = new SecondRenderer(second);
	minuteRenderer_ = new MinuteRenderer(minute);

	secondRenderer_->Initialize();
	minuteRenderer_->Initialize();
	secondRenderer_->SetParent(pTransform_);
	minuteRenderer_->SetParent(pTransform_);
	secondRenderer_->drawLayerId = RESULT_LAYER_UI;
	minuteRenderer_->drawLayerId = RESULT_LAYER_UI;


	pTransform_->scale = Vec3::kOne * 0.05f;

}

void ResultGameClearTimeRenderer::Update() {

}

