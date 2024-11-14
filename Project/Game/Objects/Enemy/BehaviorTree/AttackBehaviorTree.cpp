#include "AttackBehaviorTree.h"

#include "../Enemy.h"

EnemyBehaviorTree::AttackAction::AttackAction(Enemy* enemy,const std::string& attackName):EnemyBehaviorTree::Action(enemy),workInBehavior_(enemy->GetWorkAttack(attackName)){
	currentUpdate_ = [this](){return StartupUpdate(); };
}

EnemyBehaviorTree::AttackAction::~AttackAction(){}

EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::tick(){
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