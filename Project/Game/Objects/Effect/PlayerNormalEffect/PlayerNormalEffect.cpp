#define NOMINMAX
#include "PlayerNormalEffect.h"

/// engine
#include "FrameManager/Time.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

#include "Math/Random.h"

/// game
#include "Objects/Player/Player.h"


PlayerNormalEffect::PlayerNormalEffect(Player* _player) 
	: pPlayer_(_player) {
	CreateTag(this);
}

PlayerNormalEffect::~PlayerNormalEffect() {}

void PlayerNormalEffect::Initialize() {

	particleSystem_ = AddComponent<ParticleSystem>(1024, "PlayerNormalParticle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(24);
	particleSystem_->SetBoxEmitterMinMax({ -30.0f, 0.0f, -30.0f }, { 30.0f, 0.0f, 30.0f });
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(12.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		transform->rotate.y   += 0.05f;
		

		Vec3 diff = transform->position - pPlayer_->GetPosition();
		if(pPlayer_->GetColliderRadius() >= diff.Len()) {
			Vec3 dir = diff.Normalize();
			transform->position.x += dir.x * 10.0f * Time::DeltaTime();
			transform->position.z += dir.z * 10.0f * Time::DeltaTime();
		}
		
	});

	particleSystem_->SetPartilceStartupFunction([](Particle* _particle) {
		Transform* transform = _particle->GetTransform();
		_particle->GetMaterial().color = Vec4(Random::Vec3(Vec3::kOne * 0.5f, Vec3::kOne), 1.0f);
		transform->scale = Vec3::kOne * Random::Float(1.0f, 2.0f);
	});



	
	particleSystem_ = AddComponent<ParticleSystem>(1024, "Triangle");
	
	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(24);
	particleSystem_->SetBoxEmitterMinMax({ -30.0f, 0.0f, -30.0f }, { 30.0f, 0.0f, 30.0f });
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(12.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		//transform->position.y += 2.0f * Time::DeltaTime();
		transform->rotate.y   -= 0.02f;
		transform->scale = Vec3::kOne * std::lerp(
			0.0f, 1.5f,
			std::min((1.0f - _particle->GetNormLifeTime() * 0.3f), 1.0f)
		);


		Vec3 diff = transform->position - pPlayer_->GetPosition();
		if(pPlayer_->GetColliderRadius() >= diff.Len()) {
			Vec3 dir = diff.Normalize();
			transform->position.x += dir.x * 10.0f * Time::DeltaTime();
			transform->position.z += dir.z * 10.0f * Time::DeltaTime();
		}

		if(!_particle->GetIsAlive()) {
			transform->rotate = {};
		}
	});


	particleSystem_->SetPartilceStartupFunction([](Particle* _particle) {
		Transform* transform = _particle->GetTransform();
		_particle->GetMaterial().color = Vec4(Random::Vec3(Vec3::kOne * 0.5f, Vec3::kOne), 1.0f);
		transform->scale = Vec3::kOne * Random::Float(1.0f, 3.0f);
	});


}

void PlayerNormalEffect::Update() {

}

