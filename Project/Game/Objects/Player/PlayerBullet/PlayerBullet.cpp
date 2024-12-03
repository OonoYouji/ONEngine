#include "PlayerBullet.h"

/// engine
#include "FrameManager/Time.h"

PlayerBullet::PlayerBullet() {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {
	velocity_ = Vec3::kFront;
	lifeTime_ = 0.0f;
	isAlive_  = true;
}

void PlayerBullet::Update() {

	/// 死んでいたら更新処理をしない
	if(!isAlive_) {
		return;
	}

	/// 移動
	pTransform_->position += velocity_;

	/// 寿命を減らす
	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		isAlive_ = false;
	}
}

void PlayerBullet::StartupSetting(const Vec3& _velocity, float _maxLifeTime) {
	velocity_ = _velocity;
	lifeTime_ = _maxLifeTime;
}

