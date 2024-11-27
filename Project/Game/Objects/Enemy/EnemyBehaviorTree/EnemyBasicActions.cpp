#include "EnemyBasicActions.h"

#include <algorithm>
#include <numbers>

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../Enemy.h"
#include "../EnemyEffect/EnemyEffect.h"
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

	TransitionEffectAnimation::TransitionEffectAnimation(Enemy* enemy,
														 const std::string& animation,
														 float animationTotalTime,
														 Vector3 effectPos,
														 bool isLoop)
		:Action(enemy){
		animation_ = animation;
		animationTotalTime_ = animationTotalTime;
		effectPos_ = effectPos;
		isLoop_ = isLoop;
	}

	Status TransitionEffectAnimation::tick(){
		auto effect = enemy_->GetEnemy1Effect();
		effect->SetIsActive(true);
		enemy_->GetEnemy2Effect()->SetIsActive(false);

		effect->SetEffectAnimationRender(animation_);

		effect->SetPosition(effectPos_);

		effect->SetEffectAnimationFlags(static_cast<int>(isLoop_),isLoop_);


		if(animationTotalTime_ >= 0.0f){
			effect->SetEffectAnimationTotalTime(animationTotalTime_);
		}
		return Status::SUCCESS;
	}
	TransitionEffectAnimationWithSub::TransitionEffectAnimationWithSub(Enemy* enemy,
																	   const std::string& animation,
																	   const std::string& subAnimation,
																	   float animationTotalTime,
																	   float animationTotalTime2,
																	   Vector3 effectPos,
																	   Vector3 effect2Pos,
																	   bool isLoop,
																	   bool isLoop2)
		:Action(enemy){
		animation_[0] = animation;
		animation_[1] = subAnimation;
		effectPos_ = effectPos;
		effect2Pos_ = effect2Pos;
		animationTotalTime_[0] = animationTotalTime;
		animationTotalTime_[1] = animationTotalTime2;
		isLoop_[0] = isLoop;
		isLoop_[1] = isLoop2;
	}

	Status TransitionEffectAnimationWithSub::tick(){
		auto effect = enemy_->GetEnemy1Effect();
		effect->SetIsActive(true);
		auto effect2 = enemy_->GetEnemy2Effect();
		effect2->SetIsActive(true);

		effect->SetPosition(effectPos_);
		effect2->SetPosition(effect2Pos_);

		effect->SetEffectAnimationRender(animation_[0]);
		effect2->SetEffectAnimationRender(animation_[1]);


		effect->SetEffectAnimationFlags(static_cast<int>(isLoop_[0]),true);
		effect->SetEffectAnimationFlags(static_cast<int>(isLoop_[1]),true);

		if(animationTotalTime_[0] >= 0.0f){
			effect->SetEffectAnimationTotalTime(animationTotalTime_[0]);
		}
		if(animationTotalTime_[1] >= 0.0f){
			effect2->SetEffectAnimationTotalTime(animationTotalTime_[1]);
		}


		return Status::SUCCESS;
	}
	PlaySe::PlaySe(Enemy* enemy,const std::string& se)
		:Action(enemy){
		se_ = se;
	}

	Status PlaySe::tick(){
		enemy_->PlaySE(se_);
		return Status::SUCCESS;
	}
	PlaySe::PlaySe(Enemy* enemy,const std::string& seName)
		:Action(enemy){
		seName_ = seName;
	}
	Status PlaySe::tick(){
		enemy_->PlaySE(seName_);
		return Status::SUCCESS;
	}
}