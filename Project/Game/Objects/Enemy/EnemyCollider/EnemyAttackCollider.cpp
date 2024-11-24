#include "EnemyAttackCollider.h"

/// engine
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/Collider/SphereCollider.h"

/// objects
#include "../Enemy.h"
#include "Game/Objects/Player/Player.h"

#include "imgui.h"

EnemyAttackCollider::EnemyAttackCollider(){
	CreateTag(this);
}

EnemyAttackCollider::~EnemyAttackCollider(){

}

void EnemyAttackCollider::Initialize(){
	{
		Model* colliderModel = ModelManager::Load("Sphere");
		sphereCollider_ = AddComponent<SphereCollider>(colliderModel);

		SetParent(enemy_->GetTransform());
	}

	{
		VariableManager* vm = VariableManager::GetInstance();
		const std::string& groupName = GetTag();

		/// 値の追加
		for(size_t i = 0; i < offsetByActionTypes_.size(); i++){
			vm->AddValue(groupName,actionTypeWord[static_cast<ActionTypes>(i)].c_str(),offsetByActionTypes_[i]);
		}

		/// 読み込み
		vm->LoadSpecificGroupsToJson(
			"./Resources/Parameters/Objects",groupName
		);

		ApplyVariables();
	}
}

void EnemyAttackCollider::Update(){
	ApplyVariables();
}

void EnemyAttackCollider::OnCollisionEnter(BaseGameObject* const _collision){
	if(_collision->GetTag() == "Player"){
		Player* player = reinterpret_cast<Player*>(_collision);
		{// hp を damage 分マイナス
			// float attackedDamage = enemy_->GetDamage();
		}
	}
}

void EnemyAttackCollider::ApplyVariables(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	/// 値の追加
	for(size_t i = 0; i < offsetByActionTypes_.size(); i++){
		offsetByActionTypes_[i] = vm->GetValue<Vec3>(groupName,actionTypeWord[static_cast<ActionTypes>(i)].c_str());
	}
}

void EnemyAttackCollider::SetEnemy(Enemy* enemy){
	enemy_ = enemy;
}

void EnemyAttackCollider::Activate(ActionTypes type,float radius){
	sphereCollider_->isActive = true;
	currentUsingType_ = static_cast<int32_t>(type);
	SetPosition(offsetByActionTypes_[currentUsingType_]);
}

void EnemyAttackCollider::Terminate(){
	sphereCollider_->isActive = false;
	currentUsingType_ = -1;
}
