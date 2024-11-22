#include "EnemyWeakAttack.h"

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../../Enemy.h"
#include "../EnemyBasicActions.h"

#include "FrameManager/Time.h"

#pragma region"Startup"
EnemyBehaviorTree::WeakAttackStartup::WeakAttackStartup(Enemy* enemy,
														float startupTime,
														float collisionRadius)
	:EnemyBehaviorTree::Action(enemy){
	currentTime_ = startupTime;
	collisionRadius_ = collisionRadius;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackStartup::tick(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ <= 0.0f){
		currentTime_ = 0.0f;
		// 当たり判定を有効化
		enemy_->ActivateAttackCollider(ActionTypes::WEAK_ATTACK,collisionRadius_);
		// 次のNode へ
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"AttackAction"
EnemyBehaviorTree::WeakAttackAction::WeakAttackAction(Enemy* enemy,float activeTime,float damage)
	:EnemyBehaviorTree::Action(enemy){
	damage_ = damage;
	currentTime_ = activeTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackAction::tick(){
	currentTime_ -= Time::DeltaTime();

	if(currentTime_ <= 0.0f){
		currentTime_ = 0.0f;
		// 当たり判定を 無効化
		enemy_->TerminateAttackCollider();
		// 次のNode へ
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"EndLag"
EnemyBehaviorTree::WeakAttackEndLag::WeakAttackEndLag(Enemy* enemy,float endLagTime)
	:EnemyBehaviorTree::Action(enemy){
	currentTime_ = endLagTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackEndLag::tick(){
	currentTime_ -= Time::DeltaTime();

	if(currentTime_ <= 0.0f){
		currentTime_ = 0.0f;
		// 当たり判定を 無効化
		enemy_->TerminateAttackCollider();
		// 次のNode へ
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

EnemyBehaviorTree::WeakAttack::WeakAttack(Enemy* enemy,WorkWeakAttackAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){
	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_1",worker->motionTimes_.startupTime_,false));
	addChild(std::make_unique<WeakAttackStartup>(enemy,worker->motionTimes_.startupTime_,worker->collisionRadius_));

	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_2",worker->motionTimes_.startupTime_,false));
	addChild(std::make_unique<WeakAttackAction>(enemy,worker->motionTimes_.activeTime_,worker->damage_));

	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_3",worker->motionTimes_.startupTime_,false));
	addChild(std::make_unique<WeakAttackStartup>(enemy,worker->motionTimes_.endLagTime_,worker->damage_));
}
