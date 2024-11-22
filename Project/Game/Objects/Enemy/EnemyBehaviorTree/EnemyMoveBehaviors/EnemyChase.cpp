#include "EnemyChase.h"

#include "Math/Matrix4x4.h"

#include "../../BehaviorWorker/EnemyBehaviorWorkers.h"
#include "../EnemyBasicActions.h"
#include "../../Enemy.h"
#include "FrameManager/Time.h"
#include "Objects/Player/Player.h"

EnemyBehaviorTree::ChaseAction::ChaseAction(Enemy* enemy,WorkChaseAction* worker)
	:EnemyBehaviorTree::Action(enemy),currentTime_(0.0f),workInBehavior_(worker){
	velocity_ = Matrix4x4::Transform({0.0f,0.0f,1.0f},Matrix4x4::MakeRotateY(enemy_->GetRotate().y));
}

EnemyBehaviorTree::Status EnemyBehaviorTree::ChaseAction::tick(){
	currentTime_ += Time::DeltaTime();

	Vector3 diffP2E = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();

	Vector3 newRotate = Vector3::Slerp(Vector3::Normalize(velocity_),Vector3::Normalize(diffP2E),0.024f);
	enemy_->SetRotateY(atan2(newRotate.x,newRotate.z));
	velocity_ = Matrix4x4::Transform({0.0f,0.0f,workInBehavior_->speed_ * Time::DeltaTime()},Matrix4x4::MakeRotateY(enemy_->GetRotate().y));
	enemy_->SetPosition(enemy_->GetPosition() + velocity_);

	if(workInBehavior_->distanceToStopChasing_ > diffP2E.Len()){
		return EnemyBehaviorTree::Status::SUCCESS;
	} else if(currentTime_ >= workInBehavior_->motionTimes_.activeTime_){
		return EnemyBehaviorTree::Status::FAILURE;
	}
	return EnemyBehaviorTree::Status::RUNNING;
}

EnemyBehaviorTree::Chase::Chase(Enemy* enemy,WorkChaseAction* worker)
	:EnemyBehaviorTree::Sequence(enemy){
	addChild(std::make_unique<TransitionAnimation>(enemy,"Boss_Wait"));
	addChild(std::make_unique<ChaseAction>(enemy,worker));
}