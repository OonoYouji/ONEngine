#include "AttackBehaviorTree.h"

#include "../Enemy.h"
#include "FrameManager/Time.h"

EnemyBehaviorTree::AttackAction::AttackAction(Enemy* enemy,const std::string& attackName):EnemyBehaviorTree::Action(enemy),workInBehavior_(enemy->GetWorkAttack(attackName)){
	currentUpdate_ = [this](){return StartupUpdate(); };
	currentTime_ = 0.0f;
}

EnemyBehaviorTree::AttackAction::~AttackAction(){}

EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::tick(){
	currentTime_+= Time::DeltaTime();
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::StartupUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentUpdate_ = [this](){return Attack(); };
		return EnemyBehaviorTree::Status::RUNNING;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::Attack(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_){
		currentUpdate_ = [this](){return EndLagUpdate(); };
		return EnemyBehaviorTree::Status::RUNNING;
	}

	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::EndLagUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::Sequence(enemy){
	const std::list<ComboAttack>& variables = enemy_->GetComboAttacks(comboName);

	for(auto& variableName : variables){
		addChild(std::make_unique<AttackAction>(enemy_,comboName));
	}

}