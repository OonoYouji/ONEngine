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

void PlayerEffect::OneShotAnimation(const std::string& _filePath, float _totalTime) {
	animationRenderer_->ChangeAnimation(_filePath);
	animationRenderer_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);
	animationRenderer_->SetTotalTime(_totalTime, animationRenderer_->GetCurrentNodeAnimationKey());
	animationRenderer_->Restart();
}

