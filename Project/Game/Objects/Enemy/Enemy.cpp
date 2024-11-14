#include "Enemy.h"
#include "State/EnemyIdleState.h"
#include "State/IEnemyState.h"

#include <iostream>

#include "BehaviorTree/AttackBehaviorTree.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "Game/Objects/Player/Player.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


Enemy::Enemy(Player* player):BaseGameObject(),player_(player),maxHp_(0.0f){
	CreateTag(this);
}

Enemy::~Enemy(){}

void Enemy::Initialize(){
	animationRender_ = AddComponent<AnimationRenderer>("Kari_Boss_Wait");

	hp_ = maxHp_;

	currentState_.reset(new EnemyIdleState(this));
	currentState_->Initialize();
}

void Enemy::Update(){
	currentState_->Update();
}

void Enemy::Debug(){
	ImGui::InputText("CurrentAction :",const_cast<char*>(currentAction_.c_str()),currentAction_.size());

	///===============================================
	/// ステータスの表示
	///===============================================
	if(ImGui::TreeNode("Status")){
		ImGui::DragFloat("MaxHP",const_cast<float*>(&maxHp_),0.1f);
		ImGui::DragFloat("Speed",&speed_,0.1f);

		ImGui::Spacing();

		ImGui::InputFloat("Current HP",&hp_);

		if(ImGui::Button("SetMax Status")){
			hp_ = maxHp_;
		}
		ImGui::TreePop();
	}

	ImGui::Spacing();

	///===============================================
	/// AttackActions
	///===============================================
	if(ImGui::TreeNode("AttackActions")){
		// 新しい アクションを 生成する時に window を popupする
		if(!isCreateAttackAction_){
			if(ImGui::Button("Create Attack Action")){
				isCreateAttackAction_ = true;
			}
		} else{
			ImGui::Begin("New Attack Action");
			ImGui::InputText("New ActionName",&createObjectName_[0],sizeof(char) * 64);
			ImGui::SameLine();
			if(ImGui::Button("Create")){
				workAttackVariables_[createObjectName_] = WorkAttackAction();

				currentEditAction_ = &workAttackVariables_[createObjectName_];
				currentEditActionName_ = const_cast<std::string*>(&workAttackVariables_.find(createObjectName_)->first);
				isCreateAttackAction_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")){
				isCreateAttackAction_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::End();
		}

		ImGui::Spacing();

		// 調整できるオブジェクトが存在すれば オブジェクトを 一覧表示
		if(currentEditActionName_){
			if(ImGui::BeginCombo("Attack Actions",currentEditActionName_->c_str())){
				for(auto& [key,value] : workAttackVariables_){
					bool isSelected = (currentEditActionName_ == &key);
					if(ImGui::Selectable(key.c_str(),isSelected)){
						currentEditActionName_ = const_cast<std::string*>(&key);
						currentEditAction_ = &value;
					}
					if(isSelected){
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Spacing();

		// 調整できるように 
		if(currentEditActionName_){
			if(ImGui::TreeNode(currentEditActionName_->c_str())){
				ImGui::Spacing();
				ImGui::DragFloat("setupTime",&currentEditAction_->motionTimes_.startupTime_,0.1f,0.0f);
				ImGui::DragFloat("activeTime",&currentEditAction_->motionTimes_.activeTime_,0.1f,0.0f);
				ImGui::DragFloat("endLagTime",&currentEditAction_->motionTimes_.endLagTime_,0.1f,0.0f);
				ImGui::DragFloat("setupTime",&currentEditAction_->damage_,0.1f,0.0f);

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	///===============================================
	/// AttackCombo
	///===============================================
	if(ImGui::TreeNode("AttackCombo")){
		if(!isCreateAttackAction_){
			if(ImGui::Button("Create Attack Combo")){
				isCreateAttackCombo_ = true;
			}
		} else{
			ImGui::Begin("New Attack Combo");
			ImGui::InputText("New Combo Name",&createObjectName_[0],sizeof(char) * 64);
			ImGui::SameLine();
			if(ImGui::Button("Create")){
				workAttackVariables_[createObjectName_] = WorkAttackAction();

				currentEditAction_ = &workAttackVariables_[createObjectName_];
				currentEditActionName_ = const_cast<std::string*>(&workAttackVariables_.find(createObjectName_)->first);
				isCreateAttackAction_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel")){
				isCreateAttackAction_ = false;
				createObjectName_ = "NULL";
			}
			ImGui::End();
		}

		if(currentEditActionName_){
			if(ImGui::TreeNode(currentEditActionName_->c_str())){

				ImGui::Spacing();

				ImGui::DragFloat("setupTime",&currentEditAction_->motionTimes_.startupTime_,0.1f,0.0f);
				ImGui::DragFloat("activeTime",&currentEditAction_->motionTimes_.activeTime_,0.1f,0.0f);
				ImGui::DragFloat("endLagTime",&currentEditAction_->motionTimes_.endLagTime_,0.1f,0.0f);
				ImGui::DragFloat("setupTime",&currentEditAction_->damage_,0.1f,0.0f);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode("Action Pattern")){
		ImGui::TreePop();
	}
}

void Enemy::SetAnimationRender(const std::string& filePath){
	animationRender_ = AddComponent<AnimationRenderer>(filePath);
}

void Enemy::TransitionState(IEnemyState* next){
	currentState_.reset(next);
	currentState_->Initialize();
}

Player* Enemy::GetPlayer() const{ return player_; }

const WorkAttackAction& Enemy::GetWorkAttack(const std::string& attack) const{
	auto it = workAttackVariables_.find(attack);
	if(it != workAttackVariables_.end()){
		return it->second;  // 見つかった場合、その要素を返す
	} else{
		throw std::runtime_error("Attack action '" + attack + "' not found");  // 見つからなかった場合の例外処理
	}
}