#include "EnemyWeakAttack.h"

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../../Enemy.h"
#include "../EnemyBasicActions.h"

#include "FrameManager/Time.h"

#pragma region"Startup"
EnemyBehaviorTree::WeakAttackStartup::WeakAttackStartup(Enemy* enemy,
														float startupTime)
	:EnemyBehaviorTree::Action(enemy){
	leftTime_ = startupTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackStartup::tick(){
	leftTime_ -= Time::DeltaTime();

	if(leftTime_ <= 0.0f){
		leftTime_ = 0.0f;
		// 当たり判定を有効化
		enemy_->ActivateAttackCollider(ActionTypes::WEAK_ATTACK);

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
	leftTime_ = activeTime;

	currentColliderSwitchingTime_ = 0.0f; 
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackAction::tick(){
	leftTime_ -= Time::DeltaTime();

	enemy_->SetDamage(damage_);

	enemy_->TerminateAttackCollider();
	// SwitchingTime_ で collider を on off
	// isActiveCollider をtime で切り替える
	currentColliderSwitchingTime_ -= Time::DeltaTime();
	if(currentColliderSwitchingTime_ <= 0.0f){
		currentColliderSwitchingTime_ = colliderSwitchingTime_;
		enemy_->ActivateAttackCollider(ActionTypes::WEAK_ATTACK);
	}

	if(leftTime_ <= 0.0f){
		leftTime_ = 0.0f;
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
	leftTime_ = endLagTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttackEndLag::tick(){
	leftTime_ -= Time::DeltaTime();

	if(leftTime_ <= 0.0f){
		leftTime_ = 0.0f;
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

	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_1",worker->motionTimes_.startupTime_,true));
	addChild(std::make_unique<WeakAttackStartup>(enemy,worker->motionTimes_.startupTime_));

	addChild(std::make_unique<TransitionEffectAnimation>(enemy_,"Effect12",-1.0f,Vector3(0.0f,0.0f,0.0f),true));
	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_2",worker->motionTimes_.activeTime_,true));
	addChild(std::make_unique<WeakAttackAction>(enemy,worker->motionTimes_.activeTime_,worker->damage_));

	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_1_3",worker->motionTimes_.endLagTime_,true));
	addChild(std::make_unique<WeakAttackEndLag>(enemy,worker->motionTimes_.endLagTime_));
}
