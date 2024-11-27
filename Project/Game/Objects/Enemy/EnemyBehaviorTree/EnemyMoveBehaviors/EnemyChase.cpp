#include "EnemyChase.h"

#include <algorithm>

#include "Math/Matrix4x4.h"

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../../Enemy.h"
#include "../EnemyBasicActions.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

EnemyBehaviorTree::ChaseAction::ChaseAction(Enemy* enemy,WorkChaseAction* worker)
	:EnemyBehaviorTree::Action(enemy),currentTime_(0.0f),workInBehavior_(worker){
	velocity_ = Matrix4x4::Transform({0.0f,0.0f,1.0f},Matrix4x4::MakeRotateY(enemy_->GetRotate().y));
}

EnemyBehaviorTree::Status EnemyBehaviorTree::ChaseAction::tick(){
	currentTime_ += Time::DeltaTime();

	Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();

	// 正規化の安全性チェック
	Vector3 normalizedVelocity = velocity_.Len() > 0.0f ? Vector3::Normalize(velocity_) : Vector3();
	Vector3 normalizedDiffP2E = diffP2E.Len() > 0.0f ? Vector3::Normalize(diffP2E) : Vector3();

	// Slerp 結果の検証
	Vector3 newRotate = Vector3::Slerp(normalizedVelocity,normalizedDiffP2E,0.14f);
	if(std::isfinite(newRotate.x) && std::isfinite(newRotate.z)){
		enemy_->SetRotateY(atan2(newRotate.x,newRotate.z));
	}

	// 回転行列と速度計算の安全性チェック
	float speed = workInBehavior_->speed_ * Time::DeltaTime();
	if(std::isfinite(speed)){
		velocity_ = Matrix4x4::Transform({0.0f,0.0f,speed},
										 Matrix4x4::MakeRotateY(enemy_->GetRotate().y));
	}

	// 位置の更新
	if(std::isfinite(velocity_.x) && std::isfinite(velocity_.y) && std::isfinite(velocity_.z)){
		enemy_->SetPosition(enemy_->GetPosition() + velocity_);
	}

	if(isnan(enemy_->GetPosition().x) || isnan(enemy_->GetRotate().y)){
		enemy_->SetPosition({0.0f,0.0f,0.0f});
	}

	// 終了条件の判定
	if(workInBehavior_->distanceToStopChasing_ > diffP2E.Len()){
		return EnemyBehaviorTree::Status::SUCCESS;
	} else if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		return EnemyBehaviorTree::Status::FAILURE;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Chase::Chase(Enemy* enemy,WorkChaseAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){

	const float minChaseSpeed = 9.7f;
	const float maxChaseSpeed = 24.5f;

	const float minChaseAnimationTime_ = 0.3f;
	const float maxChaseAnimationTime_ = 1.2f;

	float animationTime = (std::lerp)(minChaseAnimationTime_,maxChaseAnimationTime_,minChaseSpeed/maxChaseSpeed);

	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Walk",animationTime,false));
	addChild(std::make_unique<ChaseAction>(enemy,worker));
}