#include "Enemy.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"



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

}

void Enemy::Update() {
	meshRenderer_->SetColor(Vec4::kRed);

	pTransform_->position += direction_ * speed_;

	lifeTime_ -= Time::DeltaTime();
}

void Enemy::OnCollisionEnter(BaseGameObject* const _collision) {
	if(_collision->GetTag() == "PlayerBullet") {
		meshRenderer_->SetColor(Vec4::kWhite);
	}
}

void Enemy::SubHP(float _subValue) {
	hp_ -= _subValue;
}
