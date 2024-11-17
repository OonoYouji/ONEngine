#include "EnemyDefeatParticle.h"

/// std
#include <numbers>

/// engine
#include "Math/Random.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// component
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

EnemyDefeatParticle::EnemyDefeatParticle() {
	CreateTag(this);
}

EnemyDefeatParticle::~EnemyDefeatParticle() {}

void EnemyDefeatParticle::Initialize() {

	lifeTime_ = 0.25f;
	const uint32_t maxParticleNum = 64;

	particleDataArray_.resize(maxParticleNum);
	for(auto& data : particleDataArray_) {
		data.direction = Random::Vec3(-Vec3::kOne, Vec3::kOne).Normalize();
		data.speed     = Random::Float(5.0f, 10.0f);
	}

	Model* star = ModelManager::Load("Star");
	star->GetMaterials().back().SetIsLighting(false);

	particleSystem_ = AddComponent<ParticleSystem>(maxParticleNum, "Star");

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform*    transform = _particle->GetTransform();
		ParticleData& data      = particleDataArray_[_particle->GetID()];

		transform->position += data.direction * data.speed * Time::DeltaTime();
		transform->rotate.z += data.speed * 0.1f * Time::DeltaTime();

		if(_particle->GetNormLifeTime() < 0.0f) {
			data.direction = Random::Vec3(-Vec3::kOne, Vec3::kOne).Normalize();
			data.speed     = Random::Float(5.0f, 10.0f);

			Vec3 rotate = 2.0f * std::numbers::pi_v<float> *Vec3::kOne;
			transform->rotate = Random::Vec3(-rotate, rotate);
		}
	});

	particleSystem_->SetParticleEmitterFlags(PARTICLE_EMITTER_BURST | PARTICLE_EMITTER_NOTIME);
	particleSystem_->SetParticleLifeTime(0.5f);
	particleSystem_->SetEmittedParticleCount(5);
	particleSystem_->SetBurst(true, lifeTime_, 0.1f);

}

void EnemyDefeatParticle::Update() {
	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ <= 0.0f) {
	}
}

