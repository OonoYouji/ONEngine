#include "EnemyRangedAttack.h"

#include "Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"

#include "Objects/EnemyBullet/IEnemyBullet.h"
#include "Objects/EnemyBulletEmitter/EnemyBulletEmitter.h"

#include "FrameManager/Time.h"

#pragma region"Startup"

EnemyBehaviorTree::RangedAttackStartup::RangedAttackStartup(Enemy* enemy,WorkRangedAttackAction* worker)
	:EnemyBehaviorTree::Action(enemy){
	worker_ = worker;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::RangedAttackStartup::tick(){
	leftTime_ -= Time::DeltaTime();

	if(leftTime_ <= 0.0f){
		// ここで Bullet Emitter 生成.
		new EnemyBulletEmitter(enemy_->GetPlayer(),enemy_,worker_->motionTimes_.activeTime_,worker_);
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"AttackAction"
EnemyBehaviorTree::RangedAttackAction::RangedAttackAction(Enemy* enemy,float activeTime)
	:EnemyBehaviorTree::Action(enemy){
	leftTime_ = activeTime;
}
EnemyBehaviorTree::Status EnemyBehaviorTree::RangedAttackAction::tick(){
	leftTime_ -= Time::DeltaTime();

	// Enemyは動かない

	if(leftTime_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"AttackAction"
EnemyBehaviorTree::RangedAttackEndLag::RangedAttackEndLag(Enemy* enemy,float endLagTime)
	:EnemyBehaviorTree::Action(enemy){
	leftTime_ = endLagTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::RangedAttackEndLag::tick(){
	leftTime_ -= Time::DeltaTime();

	// Enemyは動かない

	if(leftTime_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

EnemyBehaviorTree::RangedAttack::RangedAttack(Enemy* enemy,WorkRangedAttackAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){

	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RangedAttack_1",worker->motionTimes_.startupTime_,false));
	addChild(std::make_unique<RangedAttackStartup>(
		enemy,
		worker
	));

	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RangedAttack_2",worker->motionTimes_.activeTime_,false));
	addChild(std::make_unique<RangedAttackAction>(enemy,worker->motionTimes_.activeTime_));

	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RangedAttack_3",worker->motionTimes_.endLagTime_,false));
	addChild(std::make_unique<RangedAttackAction>(enemy,worker->motionTimes_.endLagTime_));
}