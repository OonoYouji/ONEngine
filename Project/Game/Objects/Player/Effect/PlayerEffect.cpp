#include "PlayerEffect.h"

/// components
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

PlayerEffect::PlayerEffect() {
	CreateTag(this);
}

PlayerEffect::~PlayerEffect() {}

void PlayerEffect::Initialize() {

	animationRenderer_ = AddComponent<AnimationRenderer>("Effect5");

}

void PlayerEffect::Update() {

}

void PlayerEffect::ActiveAnimation() {
	animationRenderer_->isActive = true; 
}

void PlayerEffect::SetTotalTime(float _totalTime) {
	animationRenderer_->SetTotalTime(_totalTime, animationRenderer_->GetCurrentNodeAnimationKey());
}

