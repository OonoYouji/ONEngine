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
	meshRenderer_->SetMaterial("white2x2");
	meshRenderer_->SetColor(Vec4::kRed);


	/// collider
	SphereCollider* sphereCollider = AddComponent<SphereCollider>(model);

	/// parameter
	hp_ = 10.0f;

	direction_ = Vec3::kFront;
	speed_     = 1.0f;

	moveState_ = ENEMY_MOVE_STATE_MOVE_FORWARD;
}

void Enemy::Update() {
	meshRenderer_->SetColor(Vec4::kRed);

	stateUpdateFunctionArray_[moveState_]();

	lifeTime_ -= Time::DeltaTime();
}

void Enemy::OnCollisionEnter(BaseGameObject* const _collision) {
	if(_collision->GetTag() == "PlayerBullet") {
		meshRenderer_->SetColor(Vec4::kWhite);
	}
}

void Enemy::StateMoveForward() {
	pTransform_->position += direction_ * speed_;
}

void Enemy::StateRatotion() {}

void Enemy::StateMoveUp() {
	pTransform_->position += Vec3::kUp * speed_;
}

void Enemy::SubHP(float _subValue) {
	hp_ -= _subValue;
}