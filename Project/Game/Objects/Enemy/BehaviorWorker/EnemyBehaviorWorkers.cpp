#include "EnemyBehaviorWorkers.h"

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
			bool isSelected = (animationName_ == file.second);
			if(ImGui::Selectable((file.second).c_str(),isSelected)){
				animationName_ = file.second;
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
	variableManager->SetValue(name,"collisionRadius",collisionRadius_);
	variableManager->SetValue(name,"damage",damage_);
}

void WorkWeakAttackAction::Load(const std::string& name){
	WorkEnemyAction::Load(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	damage_ = variableManager->GetValue<float>(name,"damage");
	collisionRadius_ = variableManager->GetValue<float>(name,"collisionRadius");
}

void WorkStrongAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::SliderFloat("collisionStartTime",&collisionStartTime_,0.0f,this->motionTimes_.activeTime_);
	ImGui::SliderFloat("collisionTime",&collisionTime_,0.0f,this->motionTimes_.activeTime_ - collisionStartTime_);
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateY2Player",&maxRotateY2Player_,0.0f,std::numbers::pi_v<float> *2.0f);
}

void WorkStrongAttackAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"collisionStartTime",collisionStartTime_);
	variableManager->SetValue(name,"collisionTime",collisionTime_);
	variableManager->SetValue(name,"collisionRadius",collisionRadius_);
	variableManager->SetValue(name,"damage",damage_);
	variableManager->SetValue(name,"maxRotateY2Player",maxRotateY2Player_);
}

void WorkStrongAttackAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);

	collisionStartTime_ = variableManager->GetValue<float>(name,"collisionStartTime");
	collisionTime_ = variableManager->GetValue<float>(name,"collisionTime");
	collisionRadius_ = variableManager->GetValue<float>(name,"collisionRadius");
	damage_ = variableManager->GetValue<float>(name,"damage");
	maxRotateY2Player_ = variableManager->GetValue<float>(name,"maxRotateY2Player");
}

void WorkIdleAction::Debug(){
	// Animation を 選ぶ
	if(ImGui::BeginCombo("Animations",animationName_.c_str())){
		for(auto& file : animationList){
			bool isSelected = (animationName_ == file.first);
			if(ImGui::Selectable((file.first).c_str(),isSelected)){
				animationName_ = file.first;
			}
			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
}

void WorkRushAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::DragFloat("speed",&speed_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateY2Player",&maxRotateY2Player_,0.0f,std::numbers::pi_v<float> *2.0f);
}

void WorkRushAttackAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"collisionRadius",collisionRadius_);
	variableManager->SetValue(name,"damage",damage_);
	variableManager->SetValue(name,"speed",speed_);
	variableManager->SetValue(name,"maxRotateY2Player",maxRotateY2Player_);
}

void WorkRushAttackAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);
	collisionRadius_ = variableManager->GetValue<float>(name,"collisionRadius");
	damage_          = variableManager->GetValue<float>(name,"damage");
	speed_           = variableManager->GetValue<float>(name,"speed");
	maxRotateY2Player_ = variableManager->GetValue<float>(name,"maxRotateY2Player");
}

void WorkChaseAction::Debug(){
	if(ImGui::BeginCombo("Animations",animationName_.c_str())){
		for(auto& file : animationList){
			bool isSelected = (animationName_ == file.second);
			if(ImGui::Selectable((file.second).c_str(),isSelected)){
				animationName_ = file.second;
			}
			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
	ImGui::DragFloat("distanceToStopChasing",&distanceToStopChasing_,0.1f);
	ImGui::DragFloat("speed",&speed_,0.1f);
}

void WorkChaseAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue<int>(name,"Type",static_cast<int>(type_));
	variableManager->SetValue(name,"activeTime",motionTimes_.activeTime_);
	variableManager->SetValue<float>(name,"speed",speed_);
	variableManager->SetValue<float>(name,"distanceToStopChasing",distanceToStopChasing_);
}

void WorkChaseAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	type_ = static_cast<ActionTypes>(variableManager->GetValue<int>(name,"Type"));
	motionTimes_.activeTime_ = variableManager->GetValue<float>(name,"activeTime");
	speed_ = variableManager->GetValue<float>(name,"speed");
	distanceToStopChasing_ = variableManager->GetValue<float>(name,"distanceToStopChasing");
}
