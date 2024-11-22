#include "EnemyIdle.h"

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../EnemyBasicActions.h"

#include "FrameManager/Time.h"

EnemyBehaviorTree::IdleAction::IdleAction(Enemy* enemy,WorkIdleAction* worker)
	: EnemyBehaviorTree::Action(enemy),currentTime_(0.0f){
	workInBehavior_ = worker;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::IdleAction::tick(){
	currentTime_ += Time::DeltaTime();
	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		return Status::SUCCESS;
	}
	return Status::RUNNING;
}

EnemyBehaviorTree::Idle::Idle(Enemy* enemy,WorkIdleAction* worker)
	: EnemyBehaviorTree::Sequence(enemy){
	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Wait",worker->motionTimes_.activeTime_,true));
	addChild(std::make_unique<IdleAction>(enemy,worker));
}