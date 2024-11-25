#define NOMINMAX
#include "DamageNumRender.h"

#include <numbers>

/// engine
#include "FrameManager/Time.h"

/// component
#include "ComponentManager/NumberRenderer/NumberRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"

/// scenes
#include "Scenes/Scene_Game.h"

/// math
#include "Math/Easing.h"


DamageNumRender::DamageNumRender(uint32_t _score, const Vec3& _startScale, const Vec3& _endScale, float _lerpTime, float _lifeTime, GameCamera* _gameCamera) {
	score_ = _score;

	startScale_  = _startScale;
	endScale_    = _endScale;
	lerpMaxTime_ = _lerpTime;
	lerpTime_    = 0.0f;

	lifeTime_   = _lifeTime;

	pMainCamera_ = _gameCamera;

	CreateTag(this);
}

DamageNumRender::~DamageNumRender() {}

void DamageNumRender::Initialize() {
	drawLayerId = GAME_SCENE_LAYER_NUMBER;

	pTransform_->scale = Vec3::kOne * 3.0f;
	pTransform_->rotateOrder = QUATERNION;

	numberRenderer_ = AddComponent<NumberRenderer>(5);
	numberRenderer_->SetScore(score_);

	defaultQuaternionY_ = Quaternion::MakeFromAxis(Vec3::kUp, std::numbers::pi_v<float>);
}

void DamageNumRender::Update() {

	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}


	lerpTime_ += Time::DeltaTime();
	float lerpT = std::min(lerpTime_ / lerpMaxTime_, 1.0f);


	if(score_ < 20) {

		pTransform_->scale = Vec3::Lerp(
			startScale_, endScale_, 
			Ease::Out::Circ(lerpT)
		);
	} else {

		pTransform_->scale = Vec3::Lerp(
			startScale_, endScale_, 
			Ease::Out::Back(lerpT)
		);
	}


	pTransform_->position.y += 1.0f * Time::DeltaTime();

	Vec3 direction = pMainCamera_->GetPosition() - GetPosition();
	direction.y *= 1.0f;
	pTransform_->quaternion = Quaternion::LockAt({}, direction);
	pTransform_->quaternion *= defaultQuaternionY_;

}

