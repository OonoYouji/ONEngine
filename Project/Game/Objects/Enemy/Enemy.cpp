#include "Enemy.h"

/// external
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"

/// user
#include "EnemyHPRenderer/EnemyHPRenderer.h"

Enemy::Enemy() {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	Model* model = ModelManager::Load("Enemy");

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);

	collider_ = AddComponent<SphereCollider>(model);

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

void Enemy::OnCollisionEnter(BaseGameObject* const collision) {

	

}

void Enemy::OnCollisionStay(BaseGameObject* const collision) {
	/// Enemy同士
	if(collision->GetTag() == GetTag()) {
		Vec3 pushBackDirection = pTransform_->position - collision->GetTransform()->position;
		float sumRadius = collider_->GetRadius() + collision->GetComponent<SphereCollider>()->GetRadius();
		float distance = pushBackDirection.Len();

		if(distance < sumRadius) {
			pushBackDirection.y = 0.0f;
			pushBackDirection = pushBackDirection.Normalize();
			pTransform_->position += pushBackDirection * ((sumRadius - distance) * 0.5f);

			collision->SetPosition(
				collision->GetPosition() + -pushBackDirection * ((sumRadius - distance) * 0.5f)
			);
		}
	}
}
