#include "EnemyDeadEffect.h"

/// engine
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Math/Random.h"
#include "FrameManager/Time.h"


EnemyDeadEffect::EnemyDeadEffect() {
	CreateTag(this);
}

EnemyDeadEffect::~EnemyDeadEffect() {}

void EnemyDeadEffect::Initialize() {

	/// cubeが登録されていない場合にエラーが出るので、登録する
	ModelManager::CreateCube();

	/// particle systemの初期化
	particleSystem_ = AddComponent<ParticleSystem>(kMaxParticleCount_, "Cube");
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(5.0f);
	particleSystem_->SetEmittedParticleCount(10);
	particleSystem_->SetParticleRespawnTime(0.1f);


	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {

		/// particleの情報を取得
		Transform*    transform = _particle->GetTransform();
		ParticleData& data      = particles_[_particle->GetID()];

		/// ここでparticleの更新
		transform->position += data.velocity * Time::DeltaTime();
		
		/// alphaを更新
		_particle->GetMaterial().color.w = _particle->GetNormLifeTime();


		/// 死亡判定、ここでリセットをかける
		if (!_particle->GetIsAlive()) {
			data.velocity = Random::Vec3(-Vec3::kOne, Vec3::kOne);
			data.velocity = data.velocity.Normalize() * Random::Float(1.0f, 5.0f);

			data.rotateValue = Random::Vec3(-Vec3::kOne, Vec3::kOne) * 0.25f;

			//50e4a6
			_particle->GetMaterial().color = { 0.3137f, 0.3922f, 0.6509f, 1.0f };
		}
	});



	/// particle dataの初期化
	particles_.resize(kMaxParticleCount_);
	for (auto& particle : particles_) {
		particle.velocity = Random::Vec3(-Vec3::kOne, Vec3::kOne);
		particle.velocity = particle.velocity.Normalize() * Random::Float(1.0f, 5.0f);

		particle.rotateValue = Random::Vec3(-Vec3::kOne, Vec3::kOne) * 0.25f;
	}


}

void EnemyDeadEffect::Update() {

}

