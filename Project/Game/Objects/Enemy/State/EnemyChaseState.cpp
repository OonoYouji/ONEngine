#include "../Enemy.h"
#include "EnemyAttackState.h"
#include "EnemyChaseState.h"

#include "FrameManager/Time.h"
#include "Game/Objects/Player/Player.h"
#include "Library/Math/LerpShortAngle.h"

void EnemyChaseState::Initialize(){

}

void EnemyChaseState::Update(){
	currentTime_ += Time::DeltaTime();
	if(currentTime_ >= maxTime_){
		enemy_->TransitionState(new EnemyAttackState(enemy_));
		return;
	}

	Vector3 e2pDiff = enemy_->GetPlayer()->GetPosition() - enemy_->GetPosition();
	if(e2pDiff.Len() <= gaugeLength_){
		enemy_->TransitionState(new EnemyAttackState(enemy_));
		return;
	}

	Vector3 currentRotate = enemy_->GetRotate();
	// Rotate Update
	{
		e2pDiff = e2pDiff.Normalize();
		currentRotate.y = LerpShortAngle(currentRotate.y,atan2(e2pDiff.x,e2pDiff.y),0.1f);
		enemy_->SetRotate(currentRotate);
	}

	// Postion Update
	{
		Vector3 velocity = {0.0f,0.0f,enemy_->GetSpeed()};
		velocity = Mat4::Transform(velocity,Mat4::MakeRotate(currentRotate));
		enemy_->SetPosition(enemy_->GetPosition() + velocity);
	}
}