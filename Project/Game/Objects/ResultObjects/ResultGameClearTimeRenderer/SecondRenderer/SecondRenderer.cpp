#include "SecondRenderer.h"

#include "ComponentManager/NumberRenderer/NumberRenderer.h"


SecondRenderer::SecondRenderer(float _second) 
	: timeSecond_(_second) {

	CreateTag(this);
}

SecondRenderer::~SecondRenderer() {}

void SecondRenderer::Initialize() {
	renderer_ = AddComponent<NumberRenderer>(2);

	renderer_->SetDrawDigit(2);
	renderer_->SetScore(static_cast<uint32_t>(timeSecond_));
	renderer_->SetTexture("ClearTimeText.png");

	pTransform_->position.x = 1.0f;
}

void SecondRenderer::Update() {
	renderer_->SetScore(static_cast<uint32_t>(timeSecond_));

}

void SecondRenderer::Debug() {
	ImGui::DragFloat("time", &timeSecond_, 0.2f);
}

