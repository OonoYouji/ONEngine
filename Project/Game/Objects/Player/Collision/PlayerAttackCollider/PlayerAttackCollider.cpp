#include "PlayerAttackCollider.h"

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/Collider/BoxCollider.h"

/// objects
#include "../../Player.h"
#include "Objects/DamageNumRender/DamageNumRender.h"
#include "Objects/Camera/GameCamera.h"
#include "Objects/Enemy/Enemy.h"


PlayerAttackCollider::PlayerAttackCollider(Player* _player,GameCamera* _gameCamera){
	CreateTag(this);

	pPlayer_ = _player;
	pGameCamera_ = _gameCamera;
}

PlayerAttackCollider::~PlayerAttackCollider(){}

void PlayerAttackCollider::Initialize(){

	Model* colliderModel = ModelManager::Load("Sphere");
	boxCollider_ = AddComponent<BoxCollider>(colliderModel);

	SetParent(pPlayer_->GetTransform());

	mode_ = 0;

	AddVariables();
	LoadVariables();
	ApplyVariables();
}

void PlayerAttackCollider::Update(){
	ApplyVariables();

	if(mode_ == MODE_WEAK_ATTACK){

		pTransform_->position = weakAttackTransform_.position;
		pTransform_->rotate   = weakAttackTransform_.rotate;
		colliderSize_         = weakAttackTransform_.scale;
	} else{
		pTransform_->position = strongAttackTransform_.position;
		pTransform_->rotate   = strongAttackTransform_.rotate;
		colliderSize_         = strongAttackTransform_.scale;
	}

	pTransform_->scale = Vec3::kOne * 1.0f;
	boxCollider_->SetSize(colliderSize_);
	isCollisionStay_ = false;
	isCollisionEnter_ = false;
}

void PlayerAttackCollider::Debug(){
	ImGui::Checkbox("isCollisionEnter",&isCollisionEnter_);
	ImGui::Checkbox("isCollisionStay",&isCollisionStay_);
}

void PlayerAttackCollider::OnCollisionEnter(BaseGameObject* const _collision){

	/// 衝突相手がEnemyなら
	if(_collision->GetTag() == "Enemy"){
		Enemy* enemy = static_cast<Enemy*>(_collision);
		if(enemy->GetIsInvisible()){
			return;
		}

		isCollisionEnter_ = true;

		DamageNumRender* damageRender = nullptr;
		float damage = pPlayer_->GetDamage();
		uint32_t score = static_cast<uint32_t>(damage);

		if(score < 500){
			damageRender = new DamageNumRender(
				score,
				Vec3::kOne * 0.1f,
				Vec3::kOne * 0.5f,
				0.5f,2.0f,
				pGameCamera_
			);
		} else{
			damageRender = new DamageNumRender(
				score,
				Vec3::kOne * 0.1f,
				Vec3::kOne,
				0.5f,2.0f,
				pGameCamera_
			);
		}

		damageRender->Initialize();

		Vec3 collisionPosition = _collision->GetPosition();
		Vec3 position = collisionPosition;
		position += (pGameCamera_->GetPosition() - collisionPosition).Normalize() * 0.5f;
		position += Vec3::kUp * 2.0f;

		damageRender->SetPosition(position);


		if(mode_ == MODE_WEAK_ATTACK){
			pPlayer_->PlayAudio("weakAttackCollision.wav",0.1f);
		} else{
			pPlayer_->PlayAudio("strongAttackCollision.wav",0.1f);
		}


		/// 実際にダメージを与える
		enemy->SetHP(enemy->GetHP() - damage);

		/// enemyの被弾エフェクトを出す

	}
}


void PlayerAttackCollider::AddVariables(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName,"weakA_position",weakAttackTransform_.position);
	vm->AddValue(groupName,"weakA_rotate",weakAttackTransform_.rotate);
	vm->AddValue(groupName,"weakA_scale",weakAttackTransform_.scale);

	vm->AddValue(groupName,"strongA_position",strongAttackTransform_.position);
	vm->AddValue(groupName,"strongA_rotate",strongAttackTransform_.rotate);
	vm->AddValue(groupName,"strongA_scale",strongAttackTransform_.scale);
}

void PlayerAttackCollider::LoadVariables(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects",groupName);
}

void PlayerAttackCollider::ApplyVariables(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	weakAttackTransform_.position   = vm->GetValue<Vec3>(groupName,"weakA_position");
	weakAttackTransform_.rotate     = vm->GetValue<Vec3>(groupName,"weakA_rotate");
	weakAttackTransform_.scale      = vm->GetValue<Vec3>(groupName,"weakA_scale");

	strongAttackTransform_.position = vm->GetValue<Vec3>(groupName,"strongA_position");
	strongAttackTransform_.rotate   = vm->GetValue<Vec3>(groupName,"strongA_rotate");
	strongAttackTransform_.scale    = vm->GetValue<Vec3>(groupName,"strongA_scale");
}

void PlayerAttackCollider::SetMode(int _mode){
	mode_ = _mode;
}

