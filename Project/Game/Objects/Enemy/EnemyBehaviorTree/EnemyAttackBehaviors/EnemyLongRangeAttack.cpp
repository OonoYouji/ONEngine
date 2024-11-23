#include "EnemyLongRangeAttack.h"

#include <algorithm>
#include <numbers>

#include "Objects/Enemy/BehaviorWorker/EnemyBehaviorWorkers.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyBehaviorTree/EnemyBasicActions.h"
#include "Objects/EnemyBullet/LongRangeBullet.h"
#include "Objects/Player/Player.h"

#include "FrameManager/Time.h"

namespace EnemyBehaviorTree{

#pragma region"Startup"
	LongRangeAttackStartup::LongRangeAttackStartup(Enemy* enemy,
												   float startupTime,
												   float maxRotateYSpeed)
		:Action(enemy){
		leftTime_ = startupTime;
		maxRotateYSpeed_ = maxRotateYSpeed;
	}

	Status LongRangeAttackStartup::tick(){
		leftTime_ -= Time::DeltaTime();

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

		if(leftTime_ <= 0.0f){
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
			leftBulletSpawnTime_ -= Time::DeltaTime();
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
			if(leftBulletSpawnTime_ <= 0.0f){
				// SpawnBullet
				LongRangeBullet* newBullet = new LongRangeBullet();
				newBullet->Initialize();
				Matrix4x4 rotateY = Matrix4x4::MakeRotateY(enemy_->GetRotate().y);
				Vector3 enemyDir = Matrix4x4::Transform(
					{0.0f,0.0f,1.0f},
					rotateY
				);
				newBullet->SetLifeTime(15.0f);
				newBullet->SetScale({worker_->bulletScale_,worker_->bulletScale_,worker_->bulletScale_});
				newBullet->SetPosition(enemy_->GetPosition() + (enemyDir * worker_->bulletSpawnOffsetZ_));
				newBullet->SetVelocityXZ(Vector2(enemyDir.x,enemyDir.z).Normalize() * worker_->bulletSpeed_);

				leftBulletSpawnTime_ = worker_->bulletFireInterval_;
			}
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
	}
	Status LongRangeAttackEndLag::tick(){
		leftTime_ -= Time::DeltaTime();

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

		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "1",worker->motionTimes_.startupTime_,false));
		addChild(std::make_unique<LongRangeAttackStartup>(
			enemy,
			worker->motionTimes_.startupTime_,
			worker->rotateMaxSpeed_
		));

		// Long Short を activeTime で 選ぶ
		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "2",worker->motionTimes_.activeTime_,false));
		addChild(std::make_unique<LongRangeAttackAction>(enemy,worker));

		addChild(std::make_unique<TransitionAnimationWithWeaponAndSub>(enemy,animationName + "3",worker->motionTimes_.endLagTime_,false));
		addChild(std::make_unique<LongRangeAttackEndLag>(enemy,worker->motionTimes_.endLagTime_));
	}
}