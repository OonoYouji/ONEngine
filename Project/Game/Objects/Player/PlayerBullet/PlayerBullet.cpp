#include "PlayerBullet.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"
#include "ComponentManager/Collider/BoxCollider.h"

/// game
#include "Objects/Effect/PlayerBulletHitEffect/PlayerBulletHitEffect.h"
#include "Objects/TopDownCamera/TopDownCamera.h"


PlayerBullet::PlayerBullet(TopDownCamera* _topDownCamera) 
	: pTopDownCamera_(_topDownCamera) {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {

	AddComponent<BoxCollider>(ModelManager::Load(GetTag()));

	velocity_ = Vec3::kFront;
	lifeTime_ = 0.0f;
	isAlive_  = true;
}

void PlayerBullet::Update() {

	/// 死んでいたら更新処理をしない
	if(!isAlive_) {
		return;
	}

	

	/// 寿命を減らす
	lifeTime_ -= Time::DeltaTime();

	/// 当たっていたら
	if(isHit_) {

		if(!effect_->GetIsAlive()) {
			isAlive_ = false;
			effect_->Destory();
		}

	} else {

		/// 移動
		pTransform_->position += velocity_;

		if(lifeTime_ < 0.0f) {
			isAlive_ = false;
		}

	}
}

void PlayerBullet::OnCollisionEnter(BaseGameObject* const _collision) {
	if(_collision->GetTag() == "Wall") {
		isHit_ = true;
		
		pTopDownCamera_->StartShake(0.1f, 1.0f, 1.0f);

		effect_ = new PlayerBulletHitEffect(this);
		effect_->Initialize();
		effect_->SetPosition(GetPosition());
	}
}

void PlayerBullet::StartupSetting(const Vec3& _velocity, float _maxLifeTime) {
	velocity_ = _velocity;
	lifeTime_ = _maxLifeTime;
}

