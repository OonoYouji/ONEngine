#include "Enemy.h"

/// external
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/Collider/SphereCollider.h"
#include "FrameManager/Time.h"
#include "Math/Random.h"

/// user
#include "EnemyHPRenderer/EnemyHPRenderer.h"
#include "Objects/Player/PlayerAttackCollider/PlayerAttackCollider.h"
#include "Objects/Player/Player.h"

Enemy::Enemy(Player* _player) : pPlayer_(_player) {
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

	pTransform_->position = { 0.0f, 0.0f, 10.0f };

	/// parameter setting
	maxHP_ = 100.0f;
	hp_    = maxHP_;

	stateIndex_ = 0;

}

void Enemy::Update() {

	pTransform_->rotate.y = std::atan2(
		direction_.x, direction_.z
	);

	switch(stateIndex_) {
	case 0: RootUpdate(); break;
	case 1: AttackUpdate(); break;
	}

}

void Enemy::Debug() {
	
	ImGui::DragFloat("current hp", &hp_, 0.05f);
	ImGui::DragFloat("max hp", &maxHP_,  0.05f);

	/// HPの割合を表示
	float hpRate = hp_ / maxHP_;
	ImGui::Text("HP Rate : %f", hpRate);

}

void Enemy::OnCollisionEnter(BaseGameObject* const collision) {

	if(collision->GetTag() == "PlayerAttackCollider") {
		PlayerAttackCollider* other = static_cast<PlayerAttackCollider*>(collision);
		hp_ -= other->GetDamageValue();

		/// TODO: hit effectを出す
	}

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

void Enemy::RootUpdate() {

	/// 移動する
	Vec3 diff  = pPlayer_->GetPosition() - GetPosition();
	direction_ = diff.Normalize();
	pTransform_->position += direction_ * 0.1f * Time::DeltaTime();


	if(diff.Len() < 3.0f) {
		stateIndex_ = 1;
		attackTime_ = 2.0f;
	}

}

void Enemy::AttackUpdate() {

	/// 攻撃する
	pTransform_->position += direction_ * 0.5f * Time::DeltaTime();

	/// もとに戻る
	attackTime_ -= Time::DeltaTime();
	if(attackTime_ <= 0.0f) {
		stateIndex_ = 0;
	}


}
