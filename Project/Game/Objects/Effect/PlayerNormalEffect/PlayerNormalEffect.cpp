#include "PlayerNormalEffect.h"

/// engine
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

PlayerNormalEffect::PlayerNormalEffect() {
	CreateTag(this);
}

PlayerNormalEffect::~PlayerNormalEffect() {}

void PlayerNormalEffect::Initialize() {

	particleSystem_ = AddComponent<ParticleSystem>(128, "PlayerNormalParticle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(7);
	particleSystem_->SetBoxEmitterMinMax(-Vec3::kOne * 1.5f, Vec3::kOne * 1.5f);

	pTransform_->position.y = 2.0f;

}

void PlayerNormalEffect::Update() {

}

