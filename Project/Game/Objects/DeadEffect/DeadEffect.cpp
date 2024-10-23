#include "DeadEffect.h"

/// engine
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// math
#include "Math/Random.h"


DeadEffect::DeadEffect() {
	CreateTag(this);
}

DeadEffect::~DeadEffect() {}

void DeadEffect::Initialize() {
	
	const size_t kMaxParticleCount= 12;

	particleSyste_ = AddComponent<ParticleSystem>(static_cast<uint32_t>(kMaxParticleCount), "Sphere");
	particleSyste_->SetEmittedParticleCount(1);
	particleSyste_->SetParticleLifeTime(1.0f);
	particleSyste_->SetParticleRespawnTime(0.75f);


	/// particle dataの初期化
	particleDataArray_.resize(kMaxParticleCount);
	for(auto& data : particleDataArray_) {
		data.position = Random::Vec3(-Vec3::kOne * 3.0f, Vec3::kOne * 3.0f);
		data.scaleScaler = Random::Float(0.5f, 2.0f);
	}


	/// 更新処理の関数をセットする
	particleSyste_->SetPartilceUpdateFunction([&](Particle* particle) {

		Transform* transform = particle->GetTransform();
		ParticleData& data   = particleDataArray_[particle->GetID()];

		data.scaleScaler += Time::DeltaTime() * 5.0f;

		transform->position = data.position;
		transform->scale = Vec3::kOne * data.scaleScaler;

		if(particle->GetNormLifeTime() <= 0.0f) {
			data.position    = Random::Vec3(-Vec3::kOne * 3.0f, Vec3::kOne * 3.0f);
			data.position.z  = 0.0f;
			data.scaleScaler = Random::Float(0.5f, 1.0f);
		}

	});


}

void DeadEffect::Update() {

}
