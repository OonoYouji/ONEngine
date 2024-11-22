#include "EnemyBasicActions.h"

#include <algorithm>
#include <numbers>

#include "../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../Enemy.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

EnemyBehaviorTree::TransitionAnimation::TransitionAnimation(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
	:EnemyBehaviorTree::Action(enemy){
	animation_ = animation;
	animationTotalTime_ = animationTotalTime;
	isLoop_ = isLoop;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimation::tick(){
	enemy_->SetAnimationRender(animation_);
	if(animationTotalTime_ >= 0.0f){
		enemy_->SetAnimationTotalTime(animationTotalTime_);
	}
	enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
	return Status::SUCCESS;
}

EnemyBehaviorTree::TransitionAnimationWithWeapon::TransitionAnimationWithWeapon(Enemy* enemy,const std::string& animation,float animationTotalTime,bool isLoop)
	:EnemyBehaviorTree::Action(enemy){
	animation_[0] = animation + "_B";
	animation_[1] = animation + "_W";
	animationTotalTime_ = animationTotalTime;
	isLoop_ = isLoop;
}

EnemyBehaviorTree::Status EnemyBehaviorTree::TransitionAnimationWithWeapon::tick(){
	enemy_->SetAnimationRender(animation_[0],animation_[1]);
	if(animationTotalTime_ >= 0.0f){
		enemy_->SetAnimationTotalTime(animationTotalTime_);
	}
	enemy_->SetAnimationFlags(static_cast<int>(isLoop_));
	return Status::SUCCESS;
}

/// <summary>
/// 次の State を セットする
/// </summary>
/// <param name="enemy"></param>
/// <param name="comboName"></param>
EnemyBehaviorTree::AttackCombo::AttackCombo(Enemy* enemy,const std::string& comboName):EnemyBehaviorTree::Sequence(enemy){
	const ComboAttacks& variables = enemy_->GetComboAttacks(static_cast<int32_t>(enemy_->GetHpState()),comboName);

	rangeType_ = variables.rangeType_;

	for(auto& variableName : variables.comboAttacks_){
		addChild(enemy_->CreateAction(variableName.attackName_));
	}
}

//#pragma region"TackleAttack"
//EnemyBehaviorTree::TackleAttack::TackleAttack(Enemy* enemy,WorkTackleAttackAction* worker)
//	:EnemyBehaviorTree::Action(enemy),workInBehavior_(worker),currentTime_(0.0f){
//	currentUpdate_ = [this](){return InitRotate(); };
//}
//
//EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttack::tick(){
//	return currentUpdate_();
//}
//
//EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttack::InitRotate(){
//	beforeRotateY_ = enemy_->GetRotate().y;
//
//	{
//		constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
//		// プレイヤーと敵の位置の差を計算
//		Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
//		// プレイヤー方向の角度を計算
//		float targetAngle = atan2(diffP2E.x,diffP2E.z); // atan2(x, z)で水平面の角度を求める
//
//		// 差分を計算し、2πの範囲内に正規化
//		float diff = std::fmod(beforeRotateY_ - targetAngle,maxPi);
//
//		// 最短経路の調整 (-π ～ π に収める)
//		if(diff > std::numbers::pi_v<float>){
//			diff -= maxPi;
//		} else if(diff < -std::numbers::pi_v<float>){
//			diff += maxPi;
//		}
//
//		// 最大回転角度を適用
//		rotateP2E_ -= std::clamp(diff,-workInBehavior_->maxRotateY2Player_,workInBehavior_->maxRotateY2Player_);
//	}
//
//	currentUpdate_ = [this](){return StartupUpdate(); };
//	return EnemyBehaviorTree::Status::RUNNING;
//}
//
//EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttack::StartupUpdate(){
//	currentTime_ += Time::DeltaTime();
//
//	float t = currentTime_ / workInBehavior_->motionTimes_.startupTime_;
//
//	enemy_->SetRotateY(beforeRotateY_ + std::lerp(0.0f,rotateP2E_,t));
//
//	if(currentTime_ >= workInBehavior_->motionTimes_.startupTime_){
//		currentTime_ = 0.0f;
//		currentUpdate_ = [this](){return Attack(); };
//		// 当たり判定を有効に
//		enemy_->ActivateAttackCollider(workInBehavior_->type_,workInBehavior_->collisionRadius_);
//	}
//	return EnemyBehaviorTree::Status::RUNNING;
//}
//
//EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttack::Attack(){
//	currentTime_ += Time::DeltaTime();
//
//	// Enemy の向いてる方向に 進む
//	enemy_->SetPosition(enemy_->GetPosition() + Matrix4x4::Transform({0.0f,0.0f,workInBehavior_->speed_ * Time::DeltaTime()},Matrix4x4::MakeRotateY(enemy_->GetRotate().y)));
//
//	if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
//		currentTime_ = 0.0f;
//		currentUpdate_ = [this](){return EndLagUpdate(); };
//		// 当たり判定を無効に
//		enemy_->TerminateAttackCollider();
//	}
//	return EnemyBehaviorTree::Status::RUNNING;
//}
//
//EnemyBehaviorTree::Status EnemyBehaviorTree::TackleAttack::EndLagUpdate(){
//	currentTime_ += Time::DeltaTime();
//
//	if(currentTime_ >= workInBehavior_->motionTimes_.endLagTime_){
//		currentTime_ = 0.0f;
//		return EnemyBehaviorTree::Status::SUCCESS;
//	}
//	return EnemyBehaviorTree::Status::RUNNING;
//}
//#pragma endregion

