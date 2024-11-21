#include "PlayerAvoidanceBehavior.h"

#include "PlayerRootBehavior.h"

#include "FrameManager/Time.h"
#include "Math/Vector3.h"
#include "Math/Easing.h"

PlayerAvoidanceBehavior::PlayerAvoidanceBehavior(Player* _host):
	IPlayerBehavior(_host),
	workInBehavior_(host_->GetWorkAvoidanceBehavior()){
	currentTime_ = 0.0f;

	currentUpdate_ = [this](){StartupUpdate(); };
}

void PlayerAvoidanceBehavior::Update(){
	currentTime_ += Time::DeltaTime();
	currentUpdate_();
}

void PlayerAvoidanceBehavior::StartupUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentTime_ = 0.0f;

		beforePos_ = host_->GetPosition();
		Vector3 v3Direction = {0.0f,0.0f,1.0f};
		v3Direction = Matrix4x4::Transform(v3Direction,Matrix4x4::MakeRotateY(host_->GetRotate().y));
		afterPos_ = beforePos_ + (workInBehavior_.moveDistance_ * v3Direction);

		// 無敵状態に 
		host_->SetIsInvisible(true);

		currentUpdate_ = [this](){Avoidance(); };
		Avoidance();
		return;
	}
}

void PlayerAvoidanceBehavior::Avoidance(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	host_->SetPosition(Vector3::Lerp(
		beforePos_,afterPos_,
		Ease::Out::Sine(t)
	));

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