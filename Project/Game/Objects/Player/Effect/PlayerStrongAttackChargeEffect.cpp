#include "PlayerStrongAttackChargeEffect.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"


PlayerStrongAttackChargeEffect::PlayerStrongAttackChargeEffect() {
	CreateTag(this);
}

PlayerStrongAttackChargeEffect::~PlayerStrongAttackChargeEffect() {}

void PlayerStrongAttackChargeEffect::Initialize() {

	animationRenderer_ = AddComponent<AnimationRenderer>("Effect4");
}

void PlayerStrongAttackChargeEffect::Update() {
}

void PlayerStrongAttackChargeEffect::SetTimeRate(float _timeRate) {
	animationRenderer_->SetTimeRate(_timeRate);
}

void PlayerStrongAttackChargeEffect::SetAnimationActive(bool _isActive) {
	animationRenderer_->isActive = _isActive;
}


