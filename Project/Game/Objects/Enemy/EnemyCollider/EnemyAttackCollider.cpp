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
			vm->AddValue(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Offset",offsetByActionTypes_[i]);
			vm->AddValue(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Radius",offsetByActionTypes_[i]);
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

void EnemyAttackCollider::Debug(){
#ifdef _DEBUG
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
			// float attackedDamage = enemy_->GetDamage();
		}
	}
}

void EnemyAttackCollider::ApplyVariables(){
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	/// 値の追加
	for(size_t i = 0; i < offsetByActionTypes_.size(); i++){
		offsetByActionTypes_[i] = vm->GetValue<Vec3>(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Offset");
		offsetByActionTypes_[i] = vm->GetValue<Vec3>(groupName,actionTypeWord[static_cast<ActionTypes>(i)] + "_Radius");
	}
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