#include "EnemyBasicActions.h"

#include <algorithm>
#include <numbers>

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../Enemy.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

EnemyBehaviorTree::TransitionAnimation::TransitionAnimation(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
	:EnemyBehaviorTree::Action(enemy){
	animation_ = animation;
	animationTotalTime_ = animationTotalTime;
	isLoop_ = isLoop;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimation::tick(){
	enemy_->SetAnimationRender(animation_);
	if(animationTotalTime_ >= 0.0f){
		enemy_->SetAnimationTotalTime(animationTotalTime_);
	}
	enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
	return Status::SUCCESS;
}

EnemyBehaviorTree::TransitionAnimationWithWeapon::TransitionAnimationWithWeapon(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
	:EnemyBehaviorTree::Action(enemy){
	animation_[0] = animation + "_B";
	animation_[1] = animation + "_W";
	animationTotalTime_ = animationTotalTime;
	isLoop_ = isLoop;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimationWithWeapon::tick(){
	enemy_->SetAnimationRender(animation_[0],animation_[1]);
	if(animationTotalTime_ >= 0.0f){
		enemy_->SetAnimationTotalTime(animationTotalTime_);
	}
	enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
	return Status::SUCCESS;
}

/// <summary>
/// 次の State を セットする
/// </summary>
/// <param name="enemy"></param>
/// <param name="comboName"></param>
EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::Sequence(enemy){
	const ComboAttacks& variables = enemy_->GetComboAttacks(static_cast<int32_t>(enemy_->GetHpState()),comboName);

	rangeType_ = variables.rangeType_;

	for(auto& variableName : variables.comboAttacks_){
		addChild(enemy_->CreateAction(variableName.attackName_));
	}
}