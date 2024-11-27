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
	animationRenderer_->SetIsStopAnimation(true);
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
	animationRenderer_->SetIsStopAnimation(false);

	if(_totalTime == NULL) {
		animationRenderer_->SetTimeRate(1.0f);
	} else {
		animationRenderer_->SetTotalTime(_totalTime, animationRenderer_->GetCurrentNodeAnimationKey());
	}
	
	animationRenderer_->Restart();
}

