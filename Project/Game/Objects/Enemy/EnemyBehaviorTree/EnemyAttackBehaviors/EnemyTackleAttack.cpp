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
															float lockOnTime)
	:EnemyBehaviorTree::Action(enemy){
	startupTime_ 	   = startupTime;
	currentTime_       = 0.0f;
	lockOnTime_ 	   = lockOnTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttackStartup::tick(){
	currentTime_ += Time::DeltaTime();

	// 時間内なら ロックオン
	if(currentTime_ <= lockOnTime_){
		Vector3 diffE2P = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
		enemy_->SetRotateY(std::lerp(enemy_->GetRotate().y,atan2(diffE2P.x,diffE2P.z),0.4f));
	}

	if(currentTime_ >= startupTime_){
		// 当たり判定を有効に
		enemy_->ActivateAttackCollider(ActionTypes::TACKLE_ATTACK);
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

	enemy_->SetDamage(damage_);
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

	if(leftTime_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

EnemyBehaviorTree::TackleAttack::TackleAttack(Enemy* enemy,WorkTackleAttackAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){
	// startup
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RushAttack_1",worker->motionTimes_.startupTime_,true));
	addChild(std::make_unique<TackleAttackStartup>(enemy,
			 worker->motionTimes_.startupTime_,
			 worker->lockOnTime_)
	);

	// attackAction
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RushAttack_2",worker->motionTimes_.activeTime_,true));
	addChild(std::make_unique<TackleAttackAction>(enemy,
			 worker->motionTimes_.activeTime_,
			 worker->speed_,
			 worker->damage_)
	);

	// end lag
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_RushAttack_3",worker->motionTimes_.endLagTime_,true));
	addChild(std::make_unique<TackleAttackEndLag>(enemy,
			 worker->motionTimes_.endLagTime_)
	);
}
