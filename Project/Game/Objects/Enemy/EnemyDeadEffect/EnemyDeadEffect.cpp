#include "EnemyDeadEffect.h"

/// engine
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "Math/Random.h"
#include "Math/Easing.h"
#include "FrameManager/Time.h"


EnemyDeadEffect::EnemyDeadEffect() {
	CreateTag(this);
}

EnemyDeadEffect::~EnemyDeadEffect() {}

void EnemyDeadEffect::Initialize() {

	/// cubeが登録されていない場合にエラーが出るので、登録する
	Model* cube = ModelManager::CreateCube();

	std::vector<Material>& materials = cube->GetMaterials();
	for (auto& material : materials) {
		material.SetColor({ 0.3f, 0.8f, 0.3f, 1.0f });
		material.SetIsLighting(false);
		material.SetTextureName("white2x2");
	}


	

	/// particle dataの初期化
	particles_.resize(kMaxParticleCount_);
	for (auto& particle : particles_) {
		particle.velocity = Random::Vec3(-Vec3::kOne, Vec3::kOne);
		particle.velocity = particle.velocity.Normalize() * Random::Float(5.0f, 15.0f);
		particle.scale    = Vec3::kOne * Random::Float(1.0f, 5.0f);

		particle.rotateValue = Random::Vec3(-Vec3::kOne, Vec3::kOne) * 0.25f;
	}


	/// particle systemの初期化
	particleSystem_ = AddComponent<ParticleSystem>(kMaxParticleCount_, "Cube");
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleLifeTime(2.0f);
	particleSystem_->SetEmittedParticleCount(12);
	particleSystem_->SetParticleRespawnTime(0.02f);
	//particleSystem_->SetBurst(true, 1.0f, 0.1f);


	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {

		/// particleの情報を取得
		Transform*    transform = _particle->GetTransform();
		ParticleData& data      = particles_[_particle->GetID()];

		/// ここでparticleの更新
		transform->position += data.velocity * Time::DeltaTime();

		transform->rotate += data.rotateValue * Time::DeltaTime();
		transform->scale = data.scale;
		
		/// alphaを更新
		_particle->GetMaterial().color.w = std::lerp(
			0.8f, 0.2f,
			Ease::In::Expo(_particle->GetNormLifeTime())
		);

		_particle->GetMaterial().color = { 0.3f, 0.8f, 0.3f, 0.1f };
		_particle->GetMaterial().isLighting = false;

		/// 死亡判定、ここでリセットをかける
		if (!_particle->GetIsAlive()) {
			data.velocity = Random::Vec3(-Vec3::kOne, Vec3::kOne);
			data.velocity = data.velocity.Normalize() * Random::Float(1.0f, 5.0f);

			data.rotateValue = Random::Vec3(-Vec3::kOne, Vec3::kOne) * 0.25f;

			_particle->GetMaterial().color = { 0.3f, 0.8f, 0.3f, 0.1f };

			++particleDeadCount_;
		}
	});




	thisLifeTime_      = 1.0f;
	isDestroy_         = false;
	particleDeadCount_ = 0;

}

void EnemyDeadEffect::Update() {

	thisLifeTime_ -= Time::DeltaTime();
	if (particleDeadCount_ >= static_cast<size_t>(kMaxParticleCount_)) {
		isDestroy_ = true;
	}


	/// 自身のライフタイムが終了し、パーティクルが一定期間利用されたら破棄する
	if (thisLifeTime_ < 0.0f) {
		if (isDestroy_) {
			Destory();
		}
	}

}

