#include "EnemyActions.h"

#include "../BehaviorWorker/EnemyBehaviorWorlers.h"
#include "../Enemy.h"
#include "FrameManager/Time.h"

EnemyBehaviorTree::TransitionAnimation::TransitionAnimation(Enemy* enemy,const std::string& animation)
	:EnemyBehaviorTree::Action(enemy),animation_(animation){}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimation::tick(){
	enemy_->SetAnimationRender(animation_);
	return Status::SUCCESS;
}

EnemyBehaviorTree::IdleAction::IdleAction(Enemy* enemy,WorkIdleAction* worker)
	:EnemyBehaviorTree::Action(enemy){
	workInBehavior_ = worker;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::IdleAction::tick(){
	currentTime_ += Time::DeltaTime();
	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		return Status::SUCCESS;
	}
	return Status::RUNNING;
}

EnemyBehaviorTree::WeakAttack::WeakAttack(Enemy* enemy,WorkWeakAttackAction* worker)
	:EnemyBehaviorTree::Action(enemy),workInBehavior_(worker){
	currentUpdate_ = [this](){return StartupUpdate(); };
	currentTime_ = 0.0f;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::tick(){
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::StartupUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.startupTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return Attack(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::Attack(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return EndLagUpdate(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::EndLagUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.endLagTime_){
		currentTime_ = 0.0f;
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::StrongAttack::StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker)
	:EnemyBehaviorTree::Action(enemy),workInBehavior_(worker),currentTime_(0.0f){
	currentUpdate_ = [this](){return StartupUpdate(); };
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::tick(){
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::StartupUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.startupTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return Attack(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::Attack(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return EndLagUpdate(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::EndLagUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.endLagTime_){
		currentTime_ = 0.0f;
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

/// <summary>
/// 次の State を セットする
/// </summary>
/// <param name="enemy"></param>
/// <param name="comboName"></param>
EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::Sequence(enemy){
	const ComboAttacks& variables = enemy_->GetComboAttacks(comboName);

	rangeType_ = variables.rangeType_;

	for(auto& variableName : variables.comboAttacks_){
		addChild(enemy_->CreateAction(variableName.attackName_));
	}
}