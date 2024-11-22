#include "DamageNumRender.h"

#include <numbers>

#include "FrameManager/Time.h"

/// component
#include "ComponentManager/NumberRenderer/NumberRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"



DamageNumRender::DamageNumRender(uint32_t _score, GameCamera* _gameCamera) : score_(_score), pMainCamera_(_gameCamera) {
	CreateTag(this);
}

DamageNumRender::~DamageNumRender() {}

void DamageNumRender::Initialize() {

	lifeTime_ = 5.0f;
	
	pTransform_->scale = Vec3::kOne * 3.0f;
	pTransform_->rotateOrder = QUATERNION;

	numberRenderer_ = AddComponent<NumberRenderer>(3);
	numberRenderer_->SetScore(score_);


	defaultQuaternionY_ = Quaternion::MakeFromAxis(Vec3::kUp, std::numbers::pi_v<float>);

}

void DamageNumRender::Update() {

	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}

	Vec3 direction = pMainCamera_->GetPosition() - GetPosition();
	direction.y *= 1.0f;
	pTransform_->quaternion = Quaternion::LockAt({}, direction);
	pTransform_->quaternion *= defaultQuaternionY_;

}

