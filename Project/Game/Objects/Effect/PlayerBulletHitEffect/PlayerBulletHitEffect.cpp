#include "PlayerBulletHitEffect.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// game
#include "Objects/Player/PlayerBullet/PlayerBullet.h"
#include "Objects/TopDownCamera/TopDownCamera.h"

/// math
#include "Math/Easing.h"


PlayerBulletHitEffect::PlayerBulletHitEffect(PlayerBullet* _playerBullet, TopDownCamera* _topDownCamera)
	: pPlayerBullet_(_playerBullet), pTopDownCamera_(_topDownCamera) {
	CreateTag(this);
}

PlayerBulletHitEffect::~PlayerBulletHitEffect() {}

void PlayerBulletHitEffect::Initialize() {

	explosionRenderer_ = AddComponent<MeshRenderer>();
	explosionRenderer_->SetModel("Explosion");

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");
	meshRenderer_->SetMaterial("uvChecker.png");



	particleSystem_ = AddComponent<ParticleSystem>(512, "Triangle");


	particleSystem_->SetParticleRespawnTime(0.1f);
	particleSystem_->SetEmittedParticleCount(5);
	particleSystem_->SetBoxEmitterMinMax(-Vec3::kOne, Vec3::kOne);
	particleSystem_->SetUseBillboard(false);
	particleSystem_->SetParticleEmitterFlags(PARTICLE_EMITTER_NOTIME);
	particleSystem_->SetParticleLifeTime(1.0f);

	particleSystem_->SetPartilceUpdateFunction([&](Particle* _particle) {
		Transform* transform = _particle->GetTransform();

		transform->rotate.x += 0.12f;
		transform->rotate.y += 0.25f;
		transform->scale = Vec3::kOne * std::lerp(
			1.0f, 3.0f,
			1.0f - _particle->GetNormLifeTime()
		);

		Vec3 diff = transform->position - pPlayerBullet_->GetPosition();
		if(diff.y < 0.0f) {
			diff.y *= -1.0f;
		}

		transform->position += diff.Normalize() * 30.0f * Time::DeltaTime();
	});

	particleSystem_->SetBurst(true, lifeTime_, 0.1f);


	pTransform_->scale = Vec3::kOne * 10.0f;
	pTransform_->rotateOrder = QUATERNION;
}

void PlayerBulletHitEffect::Update() {

	/*pTransform_->scale = Vec3::Lerp(
		{ 0, 0, 0 }, Vec3::kOne * 30.0f,
		Ease::In::Expo(1.0f - std::clamp(lifeTime_ / 2.0f, 0.0f, 1.0f))
	);*/

	Vec3 dir = pTopDownCamera_->GetPosition() - GetPosition();
	pTransform_->quaternion = Quaternion::LockAt({}, dir.Normalize());

	meshRenderer_->SetColor(
		{ 1.0f, 1.0f, 1.0f, 
		std::lerp(
			0.5f, 0.0f,
			Ease::In::Back(1.0f - std::clamp(lifeTime_ / 2.0f, 0.0f, 1.0f))
		) }
	);

	lifeTime_ -= Time::DeltaTime();

	isAlive_ = false;
	for(auto& particle : particleSystem_->GetParticles()) {
		if(particle->GetIsAlive()) {
			isAlive_ = true;
		}
	}

	
	if(lifeTime_ > 0.0f) {
		isAlive_ = true;
	}

}

