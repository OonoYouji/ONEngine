#include "PlayerNormalEffect.h"

/// engine
#include "FrameManager/Time.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

PlayerNormalEffect::PlayerNormalEffect() {
	CreateTag(this);
}

PlayerNormalEffect::~PlayerNormalEffect() {}

void PlayerNormalEffect::Initialize() {

	particleSystem_ = AddComponent<ParticleSystem>(1024, "PlayerNormalParticle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(7);
	particleSystem_->SetBoxEmitterMinMax(-Vec3::kOne * 1.5f, Vec3::kOne * 1.5f);
	particleSystem_->SetUseBillboard(false);

	particleSystem_->SetPartilceUpdateFunction([](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		transform->position.y += 0.25f * Time::DeltaTime();
		transform->rotate.y   += 0.01f;
		transform->scale       = Vec3::kOne * std::lerp(1.5f, 0.2f, 1.0f - _particle->GetNormLifeTime());
	});

}

void PlayerNormalEffect::Update() {

}

