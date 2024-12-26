#include "Enemy.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// user
#include "EnemyHPRenderer/EnemyHPRenderer.h"

Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Enemy");

	hpRenderer_ = new EnemyHPRenderer(this);
	hpRenderer_->Initialize();
	hpRenderer_->SetParent(pTransform_);

	pTransform_->position = { 0.0f, 0.0f, 10.0f };

	/// parameter setting
	maxHP_ = 100.0f;
	hp_    = maxHP_;
}

void Enemy::Update() {

}

