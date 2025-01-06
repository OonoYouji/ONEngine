#include "PlayerBullet.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// user
#include "Objects/Enemy/Enemy.h"


PlayerBullet::PlayerBullet() {
	CreateTag(this);
}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize() {

	Model* model = ModelManager::Load("Bullet");
	
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);

	collider_ = AddComponent<SphereCollider>(model);

	stateIndex_ = State_Idel;

}

void PlayerBullet::Update() {

	switch(stateIndex_) {
	case State_Idel: Idel(); break;
	case State_Move: Move(); break;
	}


	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ <= 0.0f) {
		Destory();
	}
}

void PlayerBullet::OnCollisionEnter(BaseGameObject* const collision) {
	if(collision->GetTag() == "Enemy") {
		isHitEnemy_ = true;
		Destory();
	}
}

void PlayerBullet::Idel() {

	pTransform_->rotate += Vec3(0.1f, 0.2f, 0.0f) * Time::DeltaTime();
	pTransform_->position = Vec3::Lerp(
		pTransform_->position, idelPosition_, 0.25f
	);
}

void PlayerBullet::Move() {

	pTransform_->position += direction_ * 100.0f * Time::DeltaTime();
}

void PlayerBullet::Fire(Enemy* _target) {
	/// 発射後のstate
	stateIndex_  = State_Move;
	targetEnemy_ = _target;

	direction_   = targetEnemy_->GetPosition() - GetPosition();
	direction_   = direction_.Normalize();
}

