#include "RotationCube.h"

// std
#include <numbers>

/// engine
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

#include "FrameManager/Time.h"
#include "Math/Random.h"

RotationCube::RotationCube() {
	CreateTag(this);
}

RotationCube::~RotationCube() {}

void RotationCube::Initialize() {

	const uint32_t kCount = 2048;

	particleDatas_.resize(kCount);
	for(size_t i = 0; i < particleDatas_.size(); ++i) {
		ParticleData& data = particleDatas_[i];
		data.offset = Random::Vec3(-Vec3::kOne, Vec3::kOne);

		data.angle = static_cast<float>(i) / kCount * 2.0f * std::numbers::pi_v<float>;
		data.phase = static_cast<float>(i) / kCount * std::numbers::pi_v<float>;
		data.rotationSpeed = Random::Float(0.05f, 0.15f) * 3.0f;
	}


	particleSystem_ = AddComponent<ParticleSystem>(kCount, "cube");

	particleSystem_->SetBoxEmitterMinMax(-Vec3::kOne, Vec3::kOne);
	particleSystem_->SetParticleRespawnTime(0.f);
	particleSystem_->SetParticleLifeTime(24.0f);
	particleSystem_->SetEmittedParticleCount(6);
	particleSystem_->SetUseBillboard(false);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();
		ParticleData& data = particleDatas_[_particle->GetID()];

		Vec3 position = {
			std::cos(data.angle) * std::cos(data.angle) * 45.0f,
			0.0f,
			std::cos(data.angle) * std::sin(data.angle) * 45.0f
		};

		position.y = std::sin(data.angle * 12.0f) * 3.0f;
		position += pTransform_->position;

		data.angle += data.rotationSpeed * Time::DeltaTime();

		position += data.offset;
		transform->position = position;

		transform->quaternion = Quaternion::MakeFromAxis(position.Normalize(), data.angle);

	});

	particleSystem_->SetPartilceStartupFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();
		ParticleData& data = particleDatas_[_particle->GetID()];

		float scaleScaler = Random::Float(0.05f, 0.2f);
		if(scaleScaler > 0.18f) {
			transform->scale = Vec3::kOne * Random::Float(0.5f, 1.0f);;
		} else {
			transform->scale = Vec3::kOne * scaleScaler;
		}

		transform->rotateOrder = QUATERNION;

		auto mate        = _particle->GetMaterial();
		mate.color       = Vec4(Random::Vec3({0.5f, 0.5f, 0.5f}, Vec3::kOne), 1.0f);
		data.offset      = Random::Vec3(-Vec3::kOne * 2.0f, Vec3::kOne * 2.0f);
		_particle->GetMaterial().color = Vec4(Random::Vec3({}, Vec3::kOne), 1.0f);
		_particle->GetMaterial().isLighting = true;
	});


	pTransform_->position.y = 5.0f;

}

void RotationCube::Update() {
	animationTime_ += Time::DeltaTime();
}

