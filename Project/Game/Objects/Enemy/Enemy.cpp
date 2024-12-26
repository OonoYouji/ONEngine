#include "Enemy.h"

/// external
#include <imgui.h>

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

void Enemy::Debug() {
	
	ImGui::DragFloat("current hp", &hp_, 0.05f);
	ImGui::DragFloat("max hp", &maxHP_,  0.05f);

	/// HPの割合を表示
	float hpRate = hp_ / maxHP_;
	ImGui::Text("HP Rate : %f", hpRate);

}