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
#include "Objects/Camera/GameCamera.h"
#include "Objects/TrackingCamera/TrackingCamera.h"
#include "Objects/HitEffect/HitEffect.h"
#include "Objects/HitEffect/HitEffectSprite.h"
#include "EnemyDeadEffect/EnemyDeadEffect.h"


Enemy::Enemy(Player* _player, EnemyManager* _manager, TrackingCamera* _camera) 
	: pPlayer_(_player), pManager_(_manager), pTrackingCamera_(_camera) {
	CreateTag(this);
}

Enemy::~Enemy() {}

void Enemy::Initialize() {

	Model* model = ModelManager::Load("Enemy");

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(model);
	meshRenderer_->SetMaterial("white2x2.png");
	meshRenderer_->SetColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

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

	if(hittedTime_ > 0.0f) {
		hittedTime_ -= Time::DeltaTime();
		if(hittedTime_ <= 0.0f) {
			meshRenderer_->SetColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
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

		meshRenderer_->SetColor(Vec4(0.75f, 0.2f, 0.2f, 1.0f));
		hittedTime_ = 0.2f;


		/// ヒットストップの設定
		Time::SetTimeRate(0.0f, 0.1f);

		{	/// ヒットエフェクトオブジェクトの生成
			/*HitEffect* hitEffect = new HitEffect(pTrackingCamera_->GetGameCamera());
			hitEffect->Initialize();
			hitEffect->SetPosition(pTransform_->position + Vec3(0, 2, 0));
			hitEffect->SetScale(Vec3::kOne * 2.0f);*/

			HitEffectSprite* hitEffectSprite = new HitEffectSprite();
			hitEffectSprite->Initialize();
			hitEffectSprite->SetPosition(pTransform_->position + Vec3(0, 2, 0));
			hitEffectSprite->UpdateMatrix();
		}
	}

	if(collision->GetTag() == "PlayerBullet") {
		hp_ -= 10.0f;
#ifdef _DEBUG /// デバッグ用高速で倒す
		hp_ -= 100.0f;
#endif // _DEBUG
		meshRenderer_->SetColor(Vec4(0.75f, 0.0f, 0.0f, 1.0f));
		hittedTime_ = 0.2f;


		/// TODO: hit effectを出す
		Explostion* explostion = new Explostion();
		explostion->Initialize();
		explostion->SetPosition(collision->GetPosition());

		pTrackingCamera_->StartShake(0.05f, 0.1f, 0.2f);

		Time::SetTimeRate(0.0f, 0.1f);
	}


	if(hp_ <= 0.0f) {
		pManager_->AddDefeatEnemiesCount();
		pManager_->DestroyEnemy(this);

		/// 死亡エフェクト
		EnemyDeadEffect* deadEffect = new EnemyDeadEffect();
		deadEffect->Initialize();
		deadEffect->SetPosition(pTransform_->position);
		deadEffect->UpdateMatrix();
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
