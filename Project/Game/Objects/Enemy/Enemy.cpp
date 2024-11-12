#include "Enemy.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"


std::array<std::function<void(void)>, ENEMY_MOVE_STATE_COUNT> Enemy::stateUpdateFunctionArray_ = {
	[]() { return &Enemy::StateMoveForward; },
	[]() { return &Enemy::StateRatotion; },
	[]() { return &Enemy::StateMoveUp; },
};


Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	Model* model = ModelManager::Load("Enemy");

	/// renderer
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("Enemy.png");

	constantColor_.w = 0.0f;

	/// collider
	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);

	/// parameter
	hp_ = 3.0f;

	direction_ = Vec3::kFront;
	speed_     = 20.0f;

	moveState_ = ENEMY_MOVE_STATE_MOVE_FORWARD;

	pTransform_->rotateOrder = QUATERNION;

}

void Enemy::Update() {


	/// 移動処理
	pTransform_->position += direction_ * speed_ * Time::DeltaTime();
	pTransform_->quaternion = Quaternion::LockAt({}, direction_);

	
	/// 色の変化
	constantColor_.w = std::lerp(
		0.0f, 0.8f, 
		std::clamp((10.0f - lifeTime_), 0.0f, 1.0f)
	);

	meshRenderer_->SetColor(constantColor_);


	/// ライフタイム
	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		Destory();
	}
}

void Enemy::OnCollisionEnter(BaseGameObject* const _collision) {
	if(_collision->GetTag() == "PlayerBullet") {
		meshRenderer_->SetColor(damagedColor_);
	}
}

void Enemy::StateMoveForward() {
	pTransform_->position += direction_ * speed_ * Time::DeltaTime();
}

void Enemy::StateRatotion() {}

void Enemy::StateMoveUp() {
	pTransform_->position += Vec3::kUp * speed_;
}

void Enemy::SubHP(float _subValue) {
	hp_ -= _subValue;
}
