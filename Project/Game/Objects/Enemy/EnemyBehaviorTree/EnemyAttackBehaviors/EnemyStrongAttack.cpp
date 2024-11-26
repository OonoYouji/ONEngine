#include "EnemyStrongAttack.h"

#include <algorithm>
#include <numbers>

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../../Enemy.h"
#include "../EnemyBasicActions.h"
#include "Game/Objects/Player/Player.h"

#include "FrameManager/Time.h"

#pragma region"Startup"
EnemyBehaviorTree::StrongAttackStartup::StrongAttackStartup(Enemy* enemy,
															float startupTime,
															float maxRotateY)
	:EnemyBehaviorTree::Action(enemy){
	beforeRotateY_ = 0.0f;
	rotateP2E_ = 0.0f;
	startupTime_ = startupTime;
	currentTime_ = 0.0f;
	currentUpdate_ = [this](){return InitRotate(); };
	maxRotateY_ = maxRotateY;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttackStartup::tick(){
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttackStartup::InitRotate(){
	beforeRotateY_ = enemy_->GetRotate().y;

	{
		constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
		// プレイヤーと敵の位置の差を計算
		Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
		// プレイヤー方向の角度を計算
		float targetAngle = atan2(diffP2E.x,diffP2E.z); // atan2(x, z)で水平面の角度を求める

		// 差分を計算し、2πの範囲内に正規化
		float diff = std::fmod(beforeRotateY_ - targetAngle,maxPi);

		// 最短経路の調整 (-π ～ π に収める)
		if(diff > std::numbers::pi_v<float>){
			diff -= maxPi;
		} else if(diff < -std::numbers::pi_v<float>){
			diff += maxPi;
		}

		// 最大回転角度を適用
		rotateP2E_ -= std::clamp(diff,-maxRotateY_,maxRotateY_);
	}

	currentUpdate_ = [this](){return Update(); };
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status  EnemyBehaviorTree::StrongAttackStartup::Update(){
	currentTime_ += Time::DeltaTime();

	float t = (std::clamp)(currentTime_ / startupTime_,0.0f,1.0f);

	enemy_->SpawnWeapon(t);

	enemy_->SetRotateY(beforeRotateY_ + std::lerp(0.0f,rotateP2E_,t));

	if(currentTime_ >= startupTime_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"AttackAction"
EnemyBehaviorTree::StrongAttackAction::StrongAttackAction(Enemy* enemy,
														  float activeTime,
														  float collisionStartTime,
														  float collisionTime,
														  float damage)
	:EnemyBehaviorTree::Action(enemy){
	activeTime_         = activeTime;
	currentTime_        = 0.0f;
	collisionStartTime_ = collisionStartTime;
	collisionTime_      = collisionTime;
	damage_             = damage;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttackAction::tick(){
	currentTime_ += Time::DeltaTime();

	// 下 の if文 を通っていないときは 攻撃判定がない 
	enemy_->TerminateAttackCollider();
	enemy_->SetDamage(0.0f);

	/*
	 collisionStartTime_ と collisionTime_ は
	 0~1 で activeTime 全体から 線形補間された数字とする
	*/
	if(activeTime_ * collisionStartTime_ <= currentTime_ && currentTime_
	   <=
	   (activeTime_ * collisionStartTime_) + (activeTime_ * collisionTime_)){
		// 当たり判定が有効
		enemy_->SetDamage(damage_);
		enemy_->ActivateAttackCollider(ActionTypes::STRONG_ATTACK);
	}

	if(currentTime_ >= activeTime_){
		// 当たり判定を無効に
		enemy_->TerminateAttackCollider();
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"EndLag"
EnemyBehaviorTree::StrongAttackEndLag::StrongAttackEndLag(Enemy* enemy,float endLagTime)
	:EnemyBehaviorTree::Action(enemy){
	currentTime_ = endLagTime;
	endLagTime_ = endLagTime;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttackEndLag::tick(){
	currentTime_ -= Time::DeltaTime();

	float t = (std::clamp)(currentTime_ / endLagTime_,0.0f,1.0f);

	enemy_->SpawnWeapon(t);

	if(currentTime_ <= 0.0f){
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

EnemyBehaviorTree::StrongAttack::StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){
	// startup
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_1",worker->motionTimes_.startupTime_,true));
	addChild(std::make_unique<StrongAttackStartup>(enemy,worker->motionTimes_.startupTime_,worker->maxRotateY2Player_));

	// attackAction
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_2",worker->motionTimes_.activeTime_,true));
	addChild(std::make_unique<StrongAttackAction>(enemy,worker->motionTimes_.activeTime_,worker->collisionStartTime_,worker->collisionTime_,worker->damage_));

	// end lag
	addChild(std::make_unique<TransitionAnimationWithWeapon>(enemy,"Boss_StrongAttack_1_3",worker->motionTimes_.endLagTime_,true));
	addChild(std::make_unique<StrongAttackEndLag>(enemy,worker->motionTimes_.endLagTime_));
}
