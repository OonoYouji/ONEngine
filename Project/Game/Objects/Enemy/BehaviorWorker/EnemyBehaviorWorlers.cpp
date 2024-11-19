#include "EnemyBehaviorWorlers.h"

#include <numbers>

#include "imgui.h"

void WorkEnemyAction::Debug(){
	ImGui::DragFloat("setupTime",&motionTimes_.startupTime_,0.1f,0.0f);
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
	ImGui::DragFloat("endLagTime",&motionTimes_.endLagTime_,0.1f,0.0f);
}

void WorkWeakAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
}

void WorkStrongAttackAction::Debug(){
	WorkEnemyAction::Debug();
	ImGui::DragFloat("collisionTime_",&collisionTime_,0.1f,0.0f);
	ImGui::DragFloat("collisionRadius",&collisionRadius_,0.1f,0.0f);
	ImGui::DragFloat("damage",&damage_,0.1f,0.0f);
	ImGui::SliderFloat("maxRotateY2Player",&maxRotateY2Player_,0.0f,std::numbers::pi_v<float> *2.0f);
}

void WorkIdleAction::Debug(){
	ImGui::DragFloat("activeTime",&motionTimes_.activeTime_,0.1f,0.0f);
}
