#include "EnemyAwakening.h"

#include "FrameManager/Time.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"

#include "ONEngine/ComponentManager/AnimationRenderer/AnimationRenderer.h"

namespace EnemyBehaviorTree{
	EnemyAwakeningAction::EnemyAwakeningAction(Enemy* enemy)
		:Action(enemy){
		leftTime_ = enemy_->GetBodyAnimationTotalTime();

		currentUpdate_ = [this](){return Init(); };
	}

	Status EnemyAwakeningAction::tick(){
		return currentUpdate_();
	}

	Status EnemyAwakeningAction::Init(){
		leftTime_ = enemy_->GetBodyAnimationTotalTime();
		currentUpdate_ = [this](){return Update(); };
		return Status::RUNNING;
	}

	Status EnemyAwakeningAction::Update(){
		leftTime_ -= Time::DeltaTime();

		if(leftTime_ <= 0.0f){
			enemy_->StartPassiveEffect();
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}

	EnemyLoadAwakeAnimation::EnemyLoadAwakeAnimation(Enemy* enemy)
		:Action(enemy){
		animation_[0] = "Boss_Awakening";
		animation_[1] = "Effect1";
		animation_[2] = "Effect2";
		animation_[3] = "Effect3";
	}
	Status EnemyLoadAwakeAnimation::tick(){
		enemy_->SetAnimationRender(animation_[0],
								   animation_[1],
								   animation_[2],
								   animation_[3]);
		// アニメーション時間で再生
		enemy_->ResetAnimationTotal();

		enemy_->SetAnimationFlags(true);
		return Status::SUCCESS;
	}

	EnemyAwakening::EnemyAwakening(Enemy* enemy)
		:Sequence(enemy){
		addChild(std::make_unique<EnemyLoadAwakeAnimation>(enemy_));
		addChild(std::make_unique<PlaySe>(enemy,"EnemySE/Awakening.wav"));
		addChild(std::make_unique<EnemyAwakeningAction>(enemy_));
	}
}