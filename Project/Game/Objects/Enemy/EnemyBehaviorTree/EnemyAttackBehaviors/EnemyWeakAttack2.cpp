#include "EnemyWeakAttack2.h"

#include <algorithm>
#include <numbers>

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../../Enemy.h"
#include "../EnemyBasicActions.h"
#include "Game/Objects/Player/Player.h"

#include "FrameManager/Time.h"

namespace EnemyBehaviorTree{
#pragma region"Startup"

	WeakAttack2Startup::WeakAttack2Startup(Enemy* enemy,float startupTime,float maxRotateY)
		:Action(enemy){
		beforeRotateY_ = 0.0f;
		rotateP2E_ = 0.0f;
		startupTime_ = startupTime;
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return InitRotate(); };
		maxRotateY_ = maxRotateY;
	}

	Status WeakAttack2Startup::tick(){
		return currentUpdate_();
	}

	Status WeakAttack2Startup::InitRotate(){
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
		return Status::RUNNING;
	}

	Status  WeakAttack2Startup::Update(){
		currentTime_ += Time::DeltaTime();

		float t = currentTime_ / startupTime_;

		enemy_->SetRotateY(beforeRotateY_ + std::lerp(0.0f,rotateP2E_,t));

		if(currentTime_ >= startupTime_){
			currentTime_ = 0.0f;
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

#pragma region"AttackAction"
	WeakAttack2Action::WeakAttack2Action(Enemy* enemy,
										 float activeTime,
										 float collisionStartTime,
										 float collisionTime,
										 float damage)
		:Action(enemy){
		activeTime_         = activeTime;
		currentTime_        = 0.0f;
		collisionStartTime_ = collisionStartTime;
		collisionTime_      = collisionTime;
		damage_             = damage;
	}

	Status WeakAttack2Action::tick(){
		currentTime_ += Time::DeltaTime();

		// 下 の if文 を通っていないときは 攻撃判定がない 
		enemy_->TerminateAttackCollider();
		if(collisionStartTime_ <= currentTime_ && currentTime_ <= collisionStartTime_ + collisionTime_){
			// 当たり判定が有効
			enemy_->ActivateAttackCollider(ActionTypes::WEAK_ATTACK_2);
		}

		enemy_->SetDamage(damage_);

		if(currentTime_ >= activeTime_){
			enemy_->SetDamage(0.0f);
			currentTime_ = 0.0f;
			// 当たり判定を無効に
			enemy_->TerminateAttackCollider();
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

#pragma region"EndLag"
	WeakAttack2EndLag::WeakAttack2EndLag(Enemy* enemy,float endLagTime)
		:Action(enemy){
		currentTime_ = endLagTime;
	}

	Status WeakAttack2EndLag::tick(){
		currentTime_ -= Time::DeltaTime();

		if(currentTime_ <= 0.0f){
			currentTime_ = 0.0f;
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

	WeakAttack2::WeakAttack2(Enemy* enemy,WorkWeakAttack2Action* worker)
		:Sequence(enemy){
		// startup
		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_2_1",worker->motionTimes_.startupTime_,true));
		addChild(std::make_unique<WeakAttack2Startup>(enemy,worker->motionTimes_.startupTime_,worker->maxRotateY2Player_));

		// attackAction
		addChild(std::make_unique<TransitionEffectAnimation>(enemy,"Effect5",-1.0f,enemy->GetCollisionOffset(ActionTypes::WEAK_ATTACK_2)));
		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_2_2",worker->motionTimes_.activeTime_,true));
		addChild(std::make_unique<WeakAttack2Action>(enemy,worker->motionTimes_.activeTime_,worker->collisionStartTime_,worker->collisionTime_,worker->damage_));

		// end lag
		addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_WeakAttack_2_3",worker->motionTimes_.endLagTime_,true));
		addChild(std::make_unique<WeakAttack2EndLag>(enemy,worker->motionTimes_.endLagTime_));
	}

}