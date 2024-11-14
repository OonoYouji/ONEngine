#include "EnemyAttackState.h"
#include "../Enemy.h"

#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"
#include "Objects/Player/Player.h"

void EnemyAttackState::Initialize(){
	// rootNode_ = ここでランダムで決める
}

void EnemyAttackState::Update(){
	EnemyBehaviorTree::Status result = rootNode_->tick();
	if(result != EnemyBehaviorTree::Status::RUNNING){
		// Idle へ
	}
}

EnemyBehaviorTree::Status ChaseAction::tick(){
	enemy_->Debug_SetCurrentAction("Chase");

	Vector3 e2pDiff = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
	if(e2pDiff.Len() <= gaugeLength_){
		return EnemyBehaviorTree::Status::SUCCESS;
	}

	Vector3 currentRotate = enemy_->GetRotate();
	Vector3 dir;
	// Rotate Update
	{
		dir = e2pDiff.Normalize();
		currentRotate.y = LerpShortAngle(currentRotate.y,atan2(dir.x,dir.y),0.1f);
		enemy_->SetRotate(currentRotate);
	}

	// Postion Update
	{
		Vector3 velocity = dir * (enemy_->GetSpeed() * Time::DeltaTime());
		enemy_->SetPosition(enemy_->GetPosition() + velocity);
	}
	return EnemyBehaviorTree::Status::RUNNING;
}