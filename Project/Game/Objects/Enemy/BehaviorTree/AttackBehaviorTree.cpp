#include "AttackBehaviorTree.h"

#include "../Enemy.h"
#include "../State/EnemyIdleState.h"
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

#pragma region"AttackAction"
/// <summary>
/// 攻撃前時間 更新
/// </summary
EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::StartupUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentUpdate_ = [this](){return Attack(); };
		return EnemyBehaviorTree::Status::RUNNING;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

/// <summary>
/// 攻撃最中時間 更新
/// </summary
EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::Attack(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_){
		currentUpdate_ = [this](){return EndLagUpdate(); };
		return EnemyBehaviorTree::Status::RUNNING;
	}

	return EnemyBehaviorTree::Status::RUNNING;
}

/// <summary>
/// 攻撃後時間 更新
/// </summary
EnemyBehaviorTree::Status EnemyBehaviorTree::AttackAction::EndLagUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

/// <summary>
/// 次の State を セットする
/// </summary>
/// <param name="enemy"></param>
/// <param name="comboName"></param>
EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::EnemyTransitionSequence(enemy,new EnemyIdleState(enemy_)){
	const ComboAttacks& variables = enemy_->GetComboAttacks(comboName);

	rangeType_ = variables.rangeType_;

	for(auto& variableName : variables.comboAttacks_){
		addChild(std::make_unique<AttackAction>(enemy_,comboName));
	}
}