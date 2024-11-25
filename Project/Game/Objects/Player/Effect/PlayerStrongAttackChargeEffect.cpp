#include "PlayerStrongAttackChargeEffect.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

#include "Math/Random.h"

PlayerStrongAttackChargeEffect::PlayerStrongAttackChargeEffect() {
	CreateTag(this);
}

PlayerStrongAttackChargeEffect::~PlayerStrongAttackChargeEffect() {}

void PlayerStrongAttackChargeEffect::Initialize() {

	//animationRenderer_ = AddComponent<AnimationRenderer>("Effect4");

	for(auto& renderer : animationRenderers_) {
		renderer = AddComponent<AnimationRenderer>("Effect4");
		renderer->SetTime(Random::Float(0.1f, 1.0f));
	}
}

void PlayerStrongAttackChargeEffect::Update() {
}

void PlayerStrongAttackChargeEffect::SetTimeRate(float _timeRate) {
	for(auto& renderer : animationRenderers_) {
		renderer->SetTimeRate(_timeRate);
	}
}

void PlayerStrongAttackChargeEffect::SetAnimationActive(bool _isActive) {
	for(auto& renderer : animationRenderers_) {
		renderer->isActive = _isActive;
	}
}


