#include "EnemyLongRangeAttack.h"

#include <algorithm>
#include <numbers>

#include "Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"
#include "Objects/Enemy/EnemyEffect/EnemyEffect.h"
#include "Objects/EnemyBullet/IEnemyBullet.h"
#include "Objects/EnemyBulletEmitter/EnemyBulletEmitter.h"
#include "Objects/Player/Player.h"

#include "FrameManager/Time.h"

namespace EnemyBehaviorTree{

#pragma region"Startup"
	LongRangeAttackStartup::LongRangeAttackStartup(Enemy* enemy,
												   WorkLongRangeAttackAction* worker,
												   float startupTime,
												   float maxRotateYSpeed)
		:Action(enemy){
		strtupTime_ = startupTime;
		leftTime_ = 0.0f;
		maxRotateYSpeed_ = maxRotateYSpeed;

		worker_ = worker;
	}

	Status LongRangeAttackStartup::tick(){
		leftTime_ += Time::DeltaTime();

		{// RotateUpdate
			Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
			float newRotateY = 0.0f;
			{
				constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
				// プレイヤーと敵の位置の差を計算
				Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
				// プレイヤー方向の角度を計算
				float targetAngle = atan2(diffP2E.x,diffP2E.z); // atan2(x, z)で水平面の角度を求める

				// 差分を計算し、2πの範囲内に正規化
				float diff = std::fmod(enemy_->GetRotate().y - targetAngle,maxPi);

				// 最短経路の調整 (-π ～ π に収める)
				if(diff > std::numbers::pi_v<float>){
					diff -= maxPi;
				} else if(diff < -std::numbers::pi_v<float>){
					diff += maxPi;
				}

				// 最大回転角度を適用
				newRotateY -= (std::clamp)(diff,-maxRotateYSpeed_,maxRotateYSpeed_) * Time::DeltaTime();
			}

			enemy_->SetRotateY(enemy_->GetRotate().y + newRotateY);
		}

		float t = (std::clamp)(leftTime_ / strtupTime_,0.0f,1.0f);

		enemy_->SpawnWeapon(t);
		enemy_->SpawnSubWeapon(t);

		if(leftTime_ >= strtupTime_){
			new EnemyBulletEmitter(
				enemy_->GetPlayer(),
				enemy_,
				worker_->motionTimes_.activeTime_,
				worker_
			);
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

#pragma region"AttackAction"
	LongRangeAttackAction::LongRangeAttackAction(Enemy* enemy,
												 WorkLongRangeAttackAction* worker)
		:Action(enemy){
		worker_ = worker;

		leftTime_ = worker->motionTimes_.activeTime_;
		leftBulletSpawnTime_ = worker->bulletFireInterval_;
	}

	Status LongRangeAttackAction::tick(){
		{
			leftTime_ -= Time::DeltaTime();
		}

		{// RotateUpdate
			Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
			float newRotateY = 0.0f;
			{
				constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
				// プレイヤーと敵の位置の差を計算
				Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
				// プレイヤー方向の角度を計算
				float targetAngle = atan2(diffP2E.x,diffP2E.z); // atan2(x, z)で水平面の角度を求める

				// 差分を計算し、2πの範囲内に正規化
				float diff = std::fmod(enemy_->GetRotate().y - targetAngle,maxPi);

				// 最短経路の調整 (-π ～ π に収める)
				if(diff > std::numbers::pi_v<float>){
					diff -= maxPi;
				} else if(diff < -std::numbers::pi_v<float>){
					diff += maxPi;
				}

				// 最大回転角度を適用
				newRotateY -= (std::clamp)(diff,-worker_->rotateMaxSpeed_,worker_->rotateMaxSpeed_) * Time::DeltaTime();
			}

			enemy_->SetRotateY(enemy_->GetRotate().y + newRotateY);
		}

		{
			if(leftTime_ <= 0.0f){
				return Status::SUCCESS;
			}
			return Status::RUNNING;
		}
	}
#pragma endregion

#pragma region"EndLag"
	LongRangeAttackEndLag::LongRangeAttackEndLag(Enemy* enemy,float endLagTime)
		:Action(enemy){
		leftTime_  = endLagTime;
		endLagTime_ = endLagTime;
	}
	Status LongRangeAttackEndLag::tick(){
		leftTime_ -= Time::DeltaTime();

		float t = (std::clamp)(leftTime_ / endLagTime_,0.0f,1.0f);

		// leftTime のはじめが endlag だから 1-t ではない
		enemy_->SpawnWeapon(t);
		enemy_->SpawnSubWeapon(t);

		if(leftTime_ <= 0.0f){
			return Status::SUCCESS;
		}
		return Status::RUNNING;
	}
#pragma endregion

	LongRangeAttack::LongRangeAttack(Enemy* enemy,WorkLongRangeAttackAction* worker)
		:Sequence(enemy){
		std::string animationName = "Boss_LongRangeAttack_";
		if(worker->motionTimes_.activeTime_ <= 5.0f){
			animationName += "half_";
		}

		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "1",worker->motionTimes_.startupTime_,true));
		addChild(std::make_unique<LongRangeAttackStartup>(
			enemy,
			worker,
			worker->motionTimes_.startupTime_,
			worker->rotateMaxSpeed_
		));

		// Long Short を activeTime で 選ぶ
		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "2",worker->motionTimes_.activeTime_,true));
		addChild(std::make_unique<PlaySe>(enemy_,"EnemySE/longRange.wav"));
		addChild(std::make_unique<LongRangeAttackAction>(enemy,worker));

		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "3",worker->motionTimes_.endLagTime_,true));
		addChild(std::make_unique<LongRangeAttackEndLag>(enemy,worker->motionTimes_.endLagTime_));
	}
	LongRangeAttack::~LongRangeAttack(){
		enemy_->GetEnemy1Effect()->SetIsActive(false);
		enemy_->GetEnemy2Effect()->SetIsActive(false);
	}
}