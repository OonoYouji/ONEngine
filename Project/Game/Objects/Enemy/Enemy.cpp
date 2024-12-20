#include "Enemy.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Enemy");

	pTransform_->position = { 0.0f, 0.0f, 10.0f };

	/// parameter setting
	maxHP_ = 100.0f;
	hp_    = maxHP_;


}

void Enemy::Update() {

}

