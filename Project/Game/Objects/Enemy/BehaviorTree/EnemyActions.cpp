#include "EnemyActions.h"

#include <algorithm>
#include <numbers>

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../Enemy.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

EnemyBehaviorTree::TransitionAnimation::TransitionAnimation(Enemy* enemy,const std::string& animation)
	:EnemyBehaviorTree::Action(enemy),animation_(animation){}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimation::tick(){
	enemy_->SetAnimationRender(animation_);
	return Status::SUCCESS;
}

#pragma region"Idle"
EnemyBehaviorTree::IdleAction::IdleAction(Enemy* enemy,WorkIdleAction* worker)
	:EnemyBehaviorTree::Action(enemy),currentTime_(0.0f){
	workInBehavior_ = worker;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::IdleAction::tick(){
	currentTime_ += Time::DeltaTime();
	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		return Status::SUCCESS;
	}
	return Status::RUNNING;
}
#pragma endregion

#pragma region"WeakAttack"
///=========================================================
/// 基本アニメーションで完結するため、あんまり書くことがない
///=========================================================
EnemyBehaviorTree::WeakAttack::WeakAttack(Enemy* enemy,WorkWeakAttackAction* worker)
	:EnemyBehaviorTree::Action(enemy),workInBehavior_(worker){
	currentUpdate_ = [this](){return StartupUpdate(); };
	currentTime_ = 0.0f;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::tick(){
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::StartupUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.startupTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return Attack(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::Attack(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return EndLagUpdate(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::WeakAttack::EndLagUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.endLagTime_){
		currentTime_ = 0.0f;
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

#pragma region"StrongAttack"
EnemyBehaviorTree::StrongAttack::StrongAttack(Enemy* enemy,WorkStrongAttackAction* worker)
	:EnemyBehaviorTree::Action(enemy),workInBehavior_(worker),currentTime_(0.0f){
	currentUpdate_ = [this](){return InitRotate(); };
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::tick(){
	return currentUpdate_();
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::InitRotate(){
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
		rotateP2E_ = std::clamp(diff,-workInBehavior_->maxRotateY2Player_,workInBehavior_->maxRotateY2Player_);
	}

	currentUpdate_ = [this](){return StartupUpdate(); };
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::StartupUpdate(){
	currentTime_ += Time::DeltaTime();

	float t = currentTime_ / workInBehavior_->motionTimes_.startupTime_;

	enemy_->SetRotateY(beforeRotateY_ + std::lerp(0.0f,rotateP2E_,t));

	if(currentTime_ >= workInBehavior_->motionTimes_.startupTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return Attack(); };
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::Attack(){
	currentTime_ += Time::DeltaTime();

	if(workInBehavior_->collisionStartTime_ <= currentTime_ && currentTime_ <= workInBehavior_->collisionStartTime_ + workInBehavior_->collisionTime_){
		// 当たり判定が有効
	}

	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		currentTime_ = 0.0f;
		currentUpdate_ = [this](){return EndLagUpdate(); };
		// 当たり判定を無効に
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::StrongAttack::EndLagUpdate(){
	currentTime_ += Time::DeltaTime();

	if(currentTime_ >= workInBehavior_->motionTimes_.endLagTime_){
		currentTime_ = 0.0f;
		return EnemyBehaviorTree::Status::SUCCESS;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}
#pragma endregion

/// <summary>
/// 次の State を セットする
/// </summary>
/// <param name="enemy"></param>
/// <param name="comboName"></param>
EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::Sequence(enemy){
	const ComboAttacks& variables = enemy_->GetComboAttacks(comboName);

	rangeType_ = variables.rangeType_;

	for(auto& variableName : variables.comboAttacks_){
		addChild(enemy_->CreateAction(variableName.attackName_));
	}
}