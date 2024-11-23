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
	ImGui::DragFloat("BulletScale",&bulletScale_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::DragFloat("timeToLand",&timeToLand_,0.1f,0.0f);
	ImGui::DragFloat("spawnCoolTime",&spawnCoolTime_,0.1f,0.0f);
	ImGui::DragFloat("spawnRange",&spawnRange_,0.1f,0.0f);
	ImGui::DragFloat("spawnPositionY",&spawnPositionY_,0.1f,0.0f);
	ImGui::DragFloat("lifeTime",&lifeTime_,0.1f,0.0f);
	ImGui::SliderFloat("attackForPlayerProbability",&attackForPlayerProbability_,0.0f,100.0f);
}

void WorkRangedAttackAction::Save(const std::string& name){
	WorkEnemyAction::Save(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue<float>(name,"bulletScale",bulletScale_);
	variableManager->SetValue<float>(name,"damage",damage_);
	variableManager->SetValue<float>(name,"timeToLand",timeToLand_);
	variableManager->SetValue<float>(name,"spawnCoolTime",spawnCoolTime_);
	variableManager->SetValue<float>(name,"spawnRange",spawnRange_);
	variableManager->SetValue<float>(name,"spawnPositionY",spawnPositionY_);
	variableManager->SetValue<float>(name,"lifeTime",lifeTime_);
	variableManager->SetValue<float>(name,"attackForPlayerProbability",attackForPlayerProbability_);
}

void WorkRangedAttackAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);
	damage_ 					= variableManager->GetValue<float>(name,"damage");
	bulletScale_ 				= variableManager->GetValue<float>(name,"bulletScale");
	timeToLand_ 				= variableManager->GetValue<float>(name,"timeToLand");
	spawnCoolTime_				= variableManager->GetValue<float>(name,"spawnCoolTime");
	spawnRange_ 				= variableManager->GetValue<float>(name,"spawnRange");
	spawnPositionY_ 			= variableManager->GetValue<float>(name,"spawnPositionY");
	lifeTime_ 					= variableManager->GetValue<float>(name,"lifeTime");
	attackForPlayerProbability_ = variableManager->GetValue<float>(name,"attackForPlayerProbability");
}

void WorkLongRangeAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::DragFloat("bulletSpeed",&bulletSpeed_,0.1f,0.0f);
	ImGui::DragFloat("bulletScale",&bulletScale_,0.1f,0.0f);
	ImGui::DragFloat("bulletFireInterval",&bulletFireInterval_,0.1f,0.0f);
	ImGui::DragFloat("bulletSpawnOffsetZ",&bulletSpawnOffsetZ_,0.1f,0.0f);
	ImGui::DragFloat("rotateMaxSpeed",&rotateMaxSpeed_,0.1f,0.0f);
}

void WorkLongRangeAttackAction::Save(const std::string& name){
	WorkEnemyAction::Save(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	variableManager->SetValue<float>(name,"damage",damage_);
	variableManager->SetValue<float>(name,"bulletSpeed",bulletSpeed_);
	variableManager->SetValue<float>(name,"bulletScale",bulletScale_);
	variableManager->SetValue<float>(name,"bulletFireInterval",bulletFireInterval_);
	variableManager->SetValue<float>(name,"bulletSpawnOffsetZ",bulletSpawnOffsetZ_);
	variableManager->SetValue<float>(name,"rotateMaxSpeed",rotateMaxSpeed_);
}

void WorkLongRangeAttackAction::Load(const std::string& name){
	WorkEnemyAction::Save(name);
	VariableManager* variableManager = VariableManager::GetInstance();
	damage_ 			= variableManager->GetValue<float>(name,"damage");
	bulletSpeed_ 		= variableManager->GetValue<float>(name,"bulletSpeed");
	bulletScale_ 		= variableManager->GetValue<float>(name,"bulletScale");
	bulletFireInterval_ = variableManager->GetValue<float>(name,"bulletFireInterval");
	bulletSpawnOffsetZ_ = variableManager->GetValue<float>(name,"bulletSpawnOffsetZ");
	rotateMaxSpeed_ 	= variableManager->GetValue<float>(name,"rotateMaxSpeed");
}

void WorkWeakAttack2Action::Debug(){
	WorkEnemyAction::Debug();
	ImGui::SliderFloat("collisionStartTime",&collisionStartTime_,0.0f,this->motionTimes_.activeTime_);
	ImGui::SliderFloat("collisionTime",&collisionTime_,0.0f,this->motionTimes_.activeTime_ - collisionStartTime_);
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateY2Player",&maxRotateY2Player_,0.0f,std::numbers::pi_v<float> *2.0f);
}

void WorkWeakAttack2Action::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"collisionStartTime",collisionStartTime_);
	variableManager->SetValue(name,"collisionTime",collisionTime_);
	variableManager->SetValue(name,"collisionRadius",collisionRadius_);
	variableManager->SetValue(name,"damage",damage_);
	variableManager->SetValue(name,"maxRotateY2Player",maxRotateY2Player_);
}

void WorkWeakAttack2Action::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);

	collisionStartTime_ = variableManager->GetValue<float>(name,"collisionStartTime");
	collisionTime_ 		= variableManager->GetValue<float>(name,"collisionTime");
	collisionRadius_ 	= variableManager->GetValue<float>(name,"collisionRadius");
	damage_ 			= variableManager->GetValue<float>(name,"damage");
	maxRotateY2Player_ 	= variableManager->GetValue<float>(name,"maxRotateY2Player");
}

void WorkJumpAwayAction::Debug(){
	ImGui::DragFloat("setupTime",&motionTimes_.startupTime_,0.1f,0.0f);
	ImGui::DragFloat("endLagTime",&motionTimes_.endLagTime_,0.1f,0.0f);
	ImGui::DragFloat("jumpSpeed",&jumpSpeed_,0.1f,0.0f);
	ImGui::DragFloat("mass",&mass_,0.1f,0.0f);
	ImGui::DragFloat("distance",&distance_,0.1f);
}

void WorkJumpAwayAction::Save(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Save(name);
	variableManager->SetValue(name,"jumpSpeed",jumpSpeed_);
	variableManager->SetValue(name,"mass",mass_);
	variableManager->SetValue(name,"distance",distance_);
}

void WorkJumpAwayAction::Load(const std::string& name){
	VariableManager* variableManager = VariableManager::GetInstance();
	WorkEnemyAction::Load(name);

	jumpSpeed_ = variableManager->GetValue<float>(name,"jumpSpeed");
	mass_ 	   = variableManager->GetValue<float>(name,"mass");
	distance_  = variableManager->GetValue<float>(name,"distance");
}
