#include "EnemyBehaviorWorkers.h"

#include <numbers>

#include "imgui.h"

#include "../Enemy.h"
#include "MyFileSystem/MyFileSystem.h"
#include "VariableManager/VariableManager.h"

void WorkEnemyAction::Debug(){
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
}

void WorkEnemyAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	type_ = static_cast<ActionTypes>(variableManager->GetValue<int>(name,"Type"));
	motionTimes_.startupTime_ = variableManager->GetValue<float>(name,"startupTime");
	motionTimes_.activeTime_ = variableManager->GetValue<float>(name,"activeTime");
	motionTimes_.endLagTime_ = variableManager->GetValue<float>(name,"endLagTime");
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
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
}

void WorkTackleAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::SliderFloat("lockOnTime",&lockOnTime_,0.0f,motionTimes_.startupTime_);
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::DragFloat("speed",&speed_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateSpeed",&maxRotateSpeed_,0.0f,std::numbers::pi_v<float> *2.0f);
	ImGui::SliderFloat("rotateSensitivity_",&rotateSensitivity_,0.0f,1.0f);
}

void WorkTackleAttackAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"lockOnTime",lockOnTime_);
	variableManager->SetValue(name,"collisionRadius",collisionRadius_);
	variableManager->SetValue(name,"damage",damage_);
	variableManager->SetValue(name,"speed",speed_);
	variableManager->SetValue(name,"maxRotateSpeed",maxRotateSpeed_);
	variableManager->SetValue(name,"rotateSensitivity",rotateSensitivity_);
}

void WorkTackleAttackAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);
	lockOnTime_ 	   = variableManager->GetValue<float>(name,"lockOnTime");
	collisionRadius_   = variableManager->GetValue<float>(name,"collisionRadius");
	damage_            = variableManager->GetValue<float>(name,"damage");
	speed_             = variableManager->GetValue<float>(name,"speed");
	maxRotateSpeed_    = variableManager->GetValue<float>(name,"maxRotateSpeed");
	rotateSensitivity_ = variableManager->GetValue<float>(name,"rotateSensitivity");
}

void WorkChaseAction::Debug(){
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

void WorkRangedAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::DragFloat("downSpeed",&downSpeed_,0.1f,0.0f);
	ImGui::DragFloat("spawnRange",&spawnRange_,0.1f,0.0f);
	ImGui::DragFloat("spawnRange",&spawnValue_,0.1f,0.0f);
	ImGui::DragFloat("spawnPositionY",&spawnPositionY_,0.1f,0.0f);
	ImGui::DragFloat("lifeTime",&lifeTime_,0.1f,0.0f);
	ImGui::DragFloat("attackForPlayerProbability",&attackForPlayerProbability_,0.1f,0.0f);
}

void WorkRangedAttackAction::Save(const std::string& name){
	WorkEnemyAction::Save(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue<float>(name,"damage",damage_);
	variableManager->SetValue<float>(name,"downSpeed",downSpeed_);
	variableManager->SetValue<float>(name,"spawnRange",spawnRange_);
	variableManager->SetValue<float>(name,"spawnValue",spawnValue_);
	variableManager->SetValue<float>(name,"spawnPositionY",spawnPositionY_);
	variableManager->SetValue<float>(name,"lifeTime",lifeTime_);
	variableManager->SetValue<float>(name,"attackForPlayerProbability",attackForPlayerProbability_);
}

void WorkRangedAttackAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);
	damage_ 					= variableManager->GetValue<float>(name,"damage");
	downSpeed_ 					= variableManager->GetValue<float>(name,"downSpeed");
	spawnRange_ 				= variableManager->GetValue<float>(name,"spawnRange");
	spawnValue_ 				= variableManager->GetValue<float>(name,"spawnValue");
	spawnPositionY_ 			= variableManager->GetValue<float>(name,"spawnPositionY");
	lifeTime_ 					= variableManager->GetValue<float>(name,"lifeTime");
	attackForPlayerProbability_ = variableManager->GetValue<float>(name,"attackForPlayerProbability");
}