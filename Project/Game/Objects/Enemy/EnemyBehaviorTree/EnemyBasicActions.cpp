#include "EnemyBasicActions.h"

#include <algorithm>
#include <numbers>

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../Enemy.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

namespace EnemyBehaviorTree{
	TransitionAnimation::TransitionAnimation(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
		:Action(enemy){
		animation_ = animation;
		animationTotalTime_ = animationTotalTime;
		isLoop_ = isLoop;
	}

	Status TransitionAnimation::tick(){
		enemy_->SetAnimationRender(animation_);
		if(animationTotalTime_ >= 0.0f){
			enemy_->SetAnimationTotalTime(animationTotalTime_);
		}
		enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
		return Status::SUCCESS;
	}

	TransitionAnimationWithWeapon::TransitionAnimationWithWeapon(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
		:Action(enemy){
		animation_[0] = animation + "_B";
		animation_[1] = animation + "_W";
		animationTotalTime_ = animationTotalTime;
		isLoop_ = isLoop;
	}

	Status TransitionAnimationWithWeapon::tick(){
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
	AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):Sequence(enemy){
		const ComboAttacks& variables = enemy_->GetComboAttacks(static_cast<int32_t>(enemy_->GetHpState()),comboName);

		rangeType_ = variables.rangeType_;

		for(auto& variableName : variables.comboAttacks_){
			addChild(enemy_->CreateAction(variableName.attackName_));
		}
	}

	TransitionAnimationWithWeaponAndSub::TransitionAnimationWithWeaponAndSub(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
		:Action(enemy){
		animation_[0] = animation + "_B";
		animation_[1] = animation + "_W1";
		animation_[2] = animation + "_W2";
		animationTotalTime_ = animationTotalTime;
		isLoop_ = isLoop;
	}
	Status TransitionAnimationWithWeaponAndSub::tick(){
		enemy_->SetAnimationRender(animation_[0],animation_[1],animation_[2]);
		if(animationTotalTime_ >= 0.0f){
			enemy_->SetAnimationTotalTime(animationTotalTime_);
		}
		enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
		return Status::SUCCESS;
	}
}