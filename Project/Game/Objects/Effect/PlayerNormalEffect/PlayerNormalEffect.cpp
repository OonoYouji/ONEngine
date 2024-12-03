#include "PlayerNormalEffect.h"

/// engine
#include "FrameManager/Time.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// game
#include "Objects/Player/Player.h"


PlayerNormalEffect::PlayerNormalEffect(Player* _player) 
	: pPlayer_(_player) {
	CreateTag(this);
}

PlayerNormalEffect::~PlayerNormalEffect() {}

void PlayerNormalEffect::Initialize() {

	particleSystem_ = AddComponent<ParticleSystem>(512, "PlayerNormalParticle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(5);
	particleSystem_->SetBoxEmitterMinMax({ -20.0f, 0.0f, -20.0f }, { 20.0f, 0.0f, 20.0f });
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(12.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		//transform->position.y += 2.0f * Time::DeltaTime();
		transform->rotate.y   += 0.01f;
		transform->scale       = Vec3::kOne * std::lerp(1.5f, 0.2f, 1.0f - _particle->GetNormLifeTime());

		Vec3 diff = transform->position - pPlayer_->GetPosition();
		if(pPlayer_->GetColliderRadius() >= diff.Len()) {
			Vec3 dir = diff.Normalize();
			transform->position.x += dir.x * 5.0f * Time::DeltaTime();
			transform->position.z += dir.z * 5.0f * Time::DeltaTime();
		}
		
	});


	
	particleSystem_ = AddComponent<ParticleSystem>(512, "Triangle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(5);
	particleSystem_->SetBoxEmitterMinMax({ -20.0f, 0.0f, -20.0f }, { 20.0f, 0.0f, 20.0f });
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(12.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		//transform->position.y += 2.0f * Time::DeltaTime();
		transform->rotate.y   -= 0.02f;
		transform->scale       = Vec3::kOne * std::lerp(3.0f, 1.0f, 1.0f - _particle->GetNormLifeTime());

		Vec3 diff = transform->position - pPlayer_->GetPosition();
		if(pPlayer_->GetColliderRadius() >= diff.Len()) {
			Vec3 dir = diff.Normalize();
			transform->position.x += dir.x * 5.0f * Time::DeltaTime();
			transform->position.z += dir.z * 5.0f * Time::DeltaTime();
		}

		if(!_particle->GetIsAlive()) {
			transform->rotate = {};
		}
	});



}

void PlayerNormalEffect::Update() {

}

