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
#include "../EnemyManager/EnemyManager.h"
#include "EnemyHPRenderer/EnemyHPRenderer.h"
#include "Objects/Player/PlayerAttackCollider/PlayerAttackCollider.h"
#include "Objects/Player/Player.h"
#include "Objects/EntityShadow/EntityShadow.h"
#include "Objects/Explostion/Explostion.h"


Enemy::Enemy(Player* _player, EnemyManager* _manager) 
	: pPlayer_(_player), pManager_(_manager) {
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

	shadow_ = new EntityShadow(this, model);
	shadow_->Initialize();

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
	case State_Root:    RootUpdate();    break;
	case State_Attack:  AttackUpdate();  break;
	case State_Blowing: BlowingUpdate(); break;
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

		blowingDirection_ = pTransform_->position - other->GetPosition();
		blowingDirection_.y = 0.0f;
		blowingDirection_ = blowingDirection_.Normalize();

		blowingTime_ = 0.1f;

		stateIndex_ = State_Blowing;

		/// TODO: hit effectを出す
	}

	if(collision->GetTag() == "PlayerBullet") {
		hp_ -= 10.0f;
#ifdef _DEBUG /// デバッグ用高速で倒す
		hp_ -= 100.0f;
#endif // _DEBUG


		/// TODO: hit effectを出す
		Explostion* explostion = new Explostion();
		explostion->Initialize();
		explostion->SetPosition(collision->GetPosition());
	}


	if(hp_ <= 0.0f) {
		pManager_->AddDefeatEnemiesCount();
		pManager_->DestroyEnemy(this);
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
	direction_ = diff;
	direction_.y = 0.0f;
	direction_ = direction_.Normalize();
	pTransform_->position += direction_ * 0.5f * Time::DeltaTime();


	if(diff.Len() < 3.0f) {
		stateIndex_ = State_Attack;
		attackTime_ = 2.0f;
	}

}

void Enemy::AttackUpdate() {

	/// 攻撃する
	pTransform_->position += direction_ * 2.0f * Time::DeltaTime();

	/// もとに戻る
	attackTime_ -= Time::DeltaTime();
	if(attackTime_ <= 0.0f) {
		stateIndex_ = State_Root;
	}


}

void Enemy::BlowingUpdate() {
	/// 吹き飛ばされる
	pTransform_->position += blowingDirection_ * 30.0f * Time::DeltaTime();

	blowingTime_ -= Time::DeltaTime();
	if(blowingTime_ <= 0.0f) {
		stateIndex_ = State_Root;
	}
}

void Enemy::Destroy() {
	Destory();
	for(auto& child : hpRenderer_->GetChilds()) {
		child->Destory();
	}
	hpRenderer_->Destory();
	shadow_->Destory();
}
