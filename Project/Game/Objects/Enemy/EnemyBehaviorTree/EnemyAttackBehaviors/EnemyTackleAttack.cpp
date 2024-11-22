#include "EnemyTackleAttack.h"

#include <algorithm>

#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"
#include "Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"
#include "Objects/Player/Player.h"

#pragma region"Setup"
EnemyBehaviorTree::TackleAttackStartup::TackleAttackStartup(Enemy* enemy,
															float startupTime,
															float lockOnTime,
															float maxRotateSpeed,
															float rotateSensitivity,
															float collisionRadius)
	:EnemyBehaviorTree::Action(enemy){
	startupTime_ 	   = startupTime;
	currentTime_       = 0.0f;
	lockOnTime_ 	   = lockOnTime;
	maxRotateSpeed_    = maxRotateSpeed;
	rotateSensitivity_ = rotateSensitivity;
	collisionRadius_   = collisionRadius;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttackStartup::tick(){
	currentTime_ += Time::DeltaTime();

	// 時間内なら ロックオン
	if(currentTime_ <= lockOnTime_){
		Vector3 diffE2P = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
		float newRotateY = LerpShortAngle(enemy_->GetRotate().y,atan2(diffE2P.x,diffE2P.y),rotateSensitivity_);
		newRotateY = (std::min)(newRotateY,maxRotateSpeed_);
		enemy_->SetRotateY(newRotateY);
	}

	if(currentTime_ >= 0.0f){
		// 当たり判定を有効に
		enemy_->ActivateAttackCollider(ActionTypes::TACKLE_ATTACK,collisionRadius_);
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"AttackAction"
EnemyBehaviorTree::
TackleAttackAction::TackleAttackAction(Enemy* enemy,
									   float activeTime,
									   float speed,
									   float damage)
	:EnemyBehaviorTree::Action(enemy){
	activeTime_ 		= activeTime;
	currentTime_		= 0.0f;
	speed_ 				= speed;
	damage_				= damage;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttackAction::tick(){
	currentTime_ += Time::DeltaTime();

	// Enemy の向いてる方向に 進む
	enemy_->SetPosition(enemy_->GetPosition() + Matrix4x4::Transform({0.0f,0.0f,speed_ * Time::DeltaTime()},Matrix4x4::MakeRotateY(enemy_->GetRotate().y)));

	if(currentTime_ >= activeTime_){
		currentTime_ = 0.0f;
		// 当たり判定を無効に
		enemy_->TerminateAttackCollider();
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"EndLag"
EnemyBehaviorTree::TackleAttackEndLag::TackleAttackEndLag(Enemy* enemy,float endLagTime)
	: EnemyBehaviorTree::Action(enemy){
	leftTime_ = endLagTime;
}
EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttackEndLag::tick(){
	leftTime_ -= Time::DeltaTime();

	if(leftTime_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

EnemyBehaviorTree::TackleAttack::TackleAttack(Enemy* enemy,WorkTackleAttackAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){
	// startup
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_1",worker->motionTimes_.startupTime_,false));
	addChild(std::make_unique<TackleAttackStartup>(enemy,
			 worker->motionTimes_.startupTime_,
			 worker->lockOnTime_,
			 worker->maxRotateSpeed_,
			 worker->rotateSensitivity_,
			 worker->collisionRadius_)
	);

	// attackAction
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_2",worker->motionTimes_.activeTime_,false));
	addChild(std::make_unique<TackleAttackAction>(enemy,
			 worker->motionTimes_.activeTime_,
			 worker->speed_,
			 worker->damage_)
	);

	// end lag
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_3",worker->motionTimes_.endLagTime_,false));
	addChild(std::make_unique<TackleAttackEndLag>(enemy,
			 worker->motionTimes_.endLagTime_)
	);
}
