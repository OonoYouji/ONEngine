#include "EnemyAwakening.h"

#include "FrameManager/Time.h"
#include "Game/Objects/Enemy/Enemy.h"
#include "Game/Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"

namespace EnemyBehaviorTree{
	EnemyAwakeningAction::EnemyAwakeningAction(Enemy* enemy)
		:Action(enemy){
		leftTime_ = enemy_->GetBodyAnimationTotalTime();
	}
	Status EnemyAwakeningAction::tick(){
		leftTime_ -= Time::DeltaTime();

		if(leftTime_ <= 0.0f){
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}

	EnemyAwakening::EnemyAwakening(Enemy* enemy)
		:Sequence(enemy){
		addChild(std::make_unique<TransitionAnimation>(enemy_,"Boss_Wait",-1.0f,false));
	}
}