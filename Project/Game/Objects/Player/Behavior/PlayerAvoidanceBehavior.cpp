#include "PlayerAvoidanceBehavior.h"
#include "../Player.h"

#include "PlayerRootBehavior.h"

#include "FrameManager/Time.h"

PlayerAvoidanceBehavior::PlayerAvoidanceBehavior(Player* _host,const Vector2& _direction):
	IPlayerBehavior(_host),
	direction_(_direction),
	workInBehavior_(host_->GetWorkAvoidanceBehavior()){
	currentTime_ = 0.0f;

	currentUpdate_ = [this](){StartupUpdate(); };
}

void PlayerAvoidanceBehavior::Update(){
	currentTime_ = Time::DeltaTime();
	currentUpdate_();
}

void PlayerAvoidanceBehavior::StartupUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentTime_ = 0.0f;

		beforePos_ = host_->GetPosition();
		afterPos_ = beforePos_ + (direction_ * workInBehavior_.moveDistance_);

		// 無敵状態に 
		host_->SetIsInvisible(true);

		currentUpdate_ = [this](){Avoidance(); };
		Avoidance();
		return;
	}
}

void PlayerAvoidanceBehavior::Avoidance(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	host_->SetPosition(Vector3::Lerp(beforePos_,afterPos_,t));

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_){
		currentTime_ = 0.0f;

		// 無敵状態 を 解除
		host_->SetIsInvisible(false);

		currentUpdate_ = [this](){EndLagUpdate(); };
		EndLagUpdate();
		return;
	}
}

void PlayerAvoidanceBehavior::EndLagUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_){
		host_->TransitionBehavior(std::make_unique<PlayerRootBehavior>(host_));
		return;
	}
}