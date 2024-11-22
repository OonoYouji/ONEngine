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


PlayerAttackCollider::PlayerAttackCollider(Player* _player, GameCamera* _gameCamera) {
	CreateTag(this);

	pPlayer_ = _player;
	pGameCamera_ = _gameCamera;
}

PlayerAttackCollider::~PlayerAttackCollider() {}

void PlayerAttackCollider::Initialize() {
	
	Model* colliderModel = ModelManager::Load("Sphere");
	boxCollider_ = AddComponent<BoxCollider>(colliderModel);

	SetParent(pPlayer_->GetTransform());


	AddVariables();
	LoadVariables();
	ApplyVariables();
}

void PlayerAttackCollider::Update() {
	ApplyVariables();

	isCollisionStay_ = false;
	isCollisionEnter_ = false;
}

void PlayerAttackCollider::Debug() {
	ImGui::Checkbox("isCollisionEnter", &isCollisionEnter_);
	ImGui::Checkbox("isCollisionStay", &isCollisionStay_);
}

void PlayerAttackCollider::OnCollisionEnter(BaseGameObject* const _collision) {

	/// 衝突相手がEnemyなら
	if(_collision->GetTag() == "Enemy") {
		isCollisionEnter_ = true;
		

		DamageNumRender* damageRender = new DamageNumRender(50, pGameCamera_);
		damageRender->Initialize();
		damageRender->SetPosition(_collision->GetPosition() + Vec3::kUp * 2.0f);
	}
}

void PlayerAttackCollider::OnCollisionStay(BaseGameObject* const _collision) {

	/// 衝突相手がEnemyなら
	if(_collision->GetTag() == "Enemy") {
		isCollisionStay_ = true;

		
	}
}



void PlayerAttackCollider::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate",   pTransform_->rotate);
	vm->AddValue(groupName, "scale",    pTransform_->scale);
}

void PlayerAttackCollider::LoadVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);
}

void PlayerAttackCollider::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

