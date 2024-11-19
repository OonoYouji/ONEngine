#include "EnemyBehaviorWorlers.h"

#include <numbers>

#include "imgui.h"

#include "../Enemy.h"
#include "MyFileSystem/MyFileSystem.h"
#include "VariableManager/VariableManager.h"

std::list<std::pair<std::string,std::string>> WorkEnemyAction::animationList = MyFileSystem::SearchFile("./Resources/Models","gltf");

void WorkEnemyAction::Debug(){
	// Animation を 選ぶ
	if(ImGui::BeginCombo("Animations",animationName_.c_str())){
		for(auto& file : animationList){
			bool isSelected = (animationName_ == file.first + "/" + file.second);
			if(ImGui::Selectable((file.first + "/" + file.second).c_str(),isSelected)){
				animationName_ = file.first + "/" + file.second;
			}
			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("setupTime",&motionTimes_.startupTime_,0.1f,0.0f);
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
	ImGui::DragFloat("endLagTime",&motionTimes_.endLagTime_,0.1f,0.0f);
}

void WorkEnemyAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue<int>(name,"Type",static_cast<int>(type_));
	variableManager->SetValue(name,"startupTime",motionTimes_.startupTime_);
	variableManager->SetValue(name,"activeTime",motionTimes_.activeTime_);
	variableManager->SetValue(name,"endLagTime",motionTimes_.endLagTime_);
	variableManager->SetValue(name,"animationName",animationName_);
}

void WorkEnemyAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	type_ = static_cast<ActionTypes>(variableManager->GetValue<int>(name,"Type"));
	motionTimes_.startupTime_ = variableManager->GetValue<float>(name,"startupTime");
	motionTimes_.activeTime_ = variableManager->GetValue<float>(name,"activeTime");
	motionTimes_.endLagTime_ = variableManager->GetValue<float>(name,"endLagTime");
	animationName_ = variableManager->GetValue<std::string>(name,"animationName");
}

void WorkWeakAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
}

void WorkWeakAttackAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"collisionRadius_",collisionRadius_);
	variableManager->SetValue(name,"damage_",damage_);
	variableManager->SaveSpecificGroupsToJson(enemyJsonDirectory,name);
}

void WorkWeakAttackAction::Load(const std::string& name){
	WorkEnemyAction::Load(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	damage_ = variableManager->GetValue<float>(name,"damage_");
	collisionRadius_ = variableManager->GetValue<float>(name,"collisionRadius_");
}

void WorkStrongAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("collisionTime_",&collisionTime_,0.1f,0.0f);
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateY2Player",&maxRotateY2Player_,0.0f,std::numbers::pi_v<float> *2.0f);
}

void WorkStrongAttackAction::Save(const std::string& name){}

void WorkStrongAttackAction::Load(const std::string& name){}

void WorkIdleAction::Debug(){
	// Animation を 選ぶ
	if(ImGui::BeginCombo("Animations",animationName_.c_str())){
		for(auto& file : animationList){
			bool isSelected = (animationName_ == file.first + "/" + file.second);
			if(ImGui::Selectable((file.first + "/" + file.second).c_str(),isSelected)){
				animationName_ = file.first + "/" + file.second;
			}
			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
}

void WorkIdleAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SaveSpecificGroupsToJson(enemyJsonDirectory,name);
}