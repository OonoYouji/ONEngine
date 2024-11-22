#include "EnemyRangedAttack.h"

#include "Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"

#include "FrameManager/Time.h"

#pragma region"Startup"
EnemyBehaviorTree::RangedAttackStartup::RangedAttackStartup(Enemy* enemy,
															float startupTime,
															float downSpeed,
															float bulletDamage,
															float bulletSpawnValue,
															float spawnRange,
															float spawnPositionY,
															float lifeTime,
															float attackForPlayerProbability)
	:EnemyBehaviorTree::Action(enemy){
	leftTime_ 						  = startupTime;
	bulletDownSpeed_ 				  = downSpeed;
	bulletDamage_ 					  = bulletDamage;
	bulletSpawnValue_                 = bulletSpawnValue;
	bulletSpawnRange_ 				  = spawnRange;
	bulletSpawnPositionY_ 			  = spawnPositionY;
	bulletLifeTime_ 				  = lifeTime;
	bulletAttackForPlayerProbability_ = attackForPlayerProbability;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::RangedAttackStartup::tick(){
	leftTime_ -= Time::DeltaTime();

	if(leftTime_ <= 0.0f){
		// ここで Bullet Emitter 生成.
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
		worker->motionTimes_.startupTime_,
		worker->downSpeed_,
		worker->damage_,
		worker->spawnValue_,
		worker->spawnRange_,
		worker->spawnPositionY_,
		worker->lifeTime_,
		worker->attackForPlayerProbability_
	));

	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RangedAttack_2",worker->motionTimes_.activeTime_,false));
	addChild(std::make_unique<RangedAttackAction>(enemy,worker->motionTimes_.activeTime_));

	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RangedAttack_3",worker->motionTimes_.endLagTime_,false));
	addChild(std::make_unique<RangedAttackAction>(enemy,worker->motionTimes_.endLagTime_));
}