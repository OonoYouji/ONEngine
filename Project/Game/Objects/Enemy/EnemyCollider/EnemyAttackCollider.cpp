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

		// 読み込み
		vm->LoadSpecificGroupsToJson(
			enemyJsonDirectory,groupName
		);

		for(size_t i = 0; i < offsetByActionTypes_.size(); i++){
			offsetByActionTypes_[i] = vm->GetValue<Vector3>(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Offset");
			radiusByActionTypes_[i] = vm->GetValue<float>(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Radius");
		}
	}
}

void EnemyAttackCollider::Update(){}

void EnemyAttackCollider::Debug(){
#ifdef _DEBUG
	if(ImGui::Button("Save")){
		Save();
	}

	// ActionType を選択する Combo
	if(ImGui::BeginCombo("Action Type",actionTypeWord[currentActionType_].c_str())){
		for(const auto& [type,name] : actionTypeWord){
			bool isSelected = (currentActionType_ == type);
			if(ImGui::Selectable(name.c_str(),isSelected)){
				currentActionType_ = type; // 現在の選択を更新
			}
			// 現在選択中の項目にフォーカスを移動
			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	int32_t currentActionTypeInt = static_cast<int>(currentActionType_);

	// 選択中の ActionType に対応する値を表示・編集
	if(currentActionTypeInt >= 0 &&
	   currentActionTypeInt <= static_cast<int>(ActionTypes::COUNT)){
		// Offset 編集用の UI
		ImGui::Text("Offset:");
		ImGui::DragFloat3("Offset Values",&offsetByActionTypes_[currentActionTypeInt].x);

		// Radius 編集用の UI
		ImGui::Text("Radius:");
		ImGui::DragFloat("Radius Value",&radiusByActionTypes_[currentActionTypeInt]);
	}
#endif // DEBUG
}

void EnemyAttackCollider::OnCollisionEnter(BaseGameObject* const _collision){
	if(_collision->GetTag() == "Player"){
		Player* player = reinterpret_cast<Player*>(_collision);
		{// hp を damage 分マイナス
			if(player->GetIsInvisible()){
				return;
			}

			float damage = enemy_->GetDamage();
			player->SetHp(player->GetCurrentHP() - damage);

			///　シェイク、ビネット
			float scale = damage / 250.0f;
			player->DamageEffectStart(
				1.0f, damage,
				0.1f * scale, 5.3f * scale
			);
		}
	}
}

void EnemyAttackCollider::Save(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	for(size_t i = 0; i < offsetByActionTypes_.size(); i++){
		vm->SetValue(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Offset",offsetByActionTypes_[i]);
		vm->SetValue(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Radius",radiusByActionTypes_[i]);
	}

	vm->SaveSpecificGroupsToJson(enemyJsonDirectory,groupName);
}

void EnemyAttackCollider::SetEnemy(Enemy* enemy){
	enemy_ = enemy;
}

void EnemyAttackCollider::Activate(ActionTypes type){
	sphereCollider_->isActive = true;
	currentUsingType_ = static_cast<int32_t>(type);

	SetPosition(offsetByActionTypes_[currentUsingType_]);

	sphereCollider_->SetRadius(radiusByActionTypes_[currentUsingType_]);
}

void EnemyAttackCollider::Terminate(){
	sphereCollider_->isActive = false;
	currentUsingType_ = -1;
}
