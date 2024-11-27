#include "PlayerEffect.h"

/// components
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

PlayerEffect::PlayerEffect() {
	CreateTag(this);
}

PlayerEffect::~PlayerEffect() {}

void PlayerEffect::Initialize() {

	animationRenderer_ = AddComponent<AnimationRenderer>("Effect9");
	animationRenderer_->SetTimeRate(0.0f);
	animationRenderer_->isActive = false;
}

void PlayerEffect::Update() {
	float animationTime = animationRenderer_->GetAnimationTime();
	if(animationTime / totalTime_ >= 1.0f) {
		animationRenderer_->isActive = false;
	}
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
	animationRenderer_->isActive = true;

	if(_totalTime == NULL) {
		animationRenderer_->SetTimeRate(1.0f);
		totalTime_ = 1.0f;
	} else {
		animationRenderer_->SetTotalTime(_totalTime, animationRenderer_->GetCurrentNodeAnimationKey());
		totalTime_ = _totalTime;
	}
	
	animationRenderer_->Restart();
}

