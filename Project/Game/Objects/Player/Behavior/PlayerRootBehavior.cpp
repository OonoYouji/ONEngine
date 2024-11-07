#include "../Player.h"
#include "PlayerRootBehavior.h"

#include "PlayerAvoidanceBehavior.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"
#include "Library/Math/LerpShortAngle.h"

PlayerRootBehavior::PlayerRootBehavior(Player* _host):IPlayerBehavior(_host),workInBehavior_(host_->GetWorkRootBehavior()){}

void PlayerRootBehavior::Update(){
	direction_ = {
		static_cast<float>(Input::PressKey(KeyCode::D)) - static_cast<float>(Input::PressKey(KeyCode::A)),
		static_cast<float>(Input::PressKey(KeyCode::W)) - static_cast<float>(Input::PressKey(KeyCode::S))
	};
	direction_ += Input::GetLeftStick();

	direction_ = direction_.Normalize();

	// Rotate Update
	{
		if(direction_.x != 0 || direction_.y != 0){
			lastDir_ = direction_;
		}
		Vector3 rotate = host_->GetRotate();
		rotate.y = lerpShortAngle(rotate.y,atan2(lastDir_.x,lastDir_.y),workInBehavior_.rotateLerpSensitivity_);
		host_->SetRotate(rotate);
	}

	{  // Postion Update
		float playerSpeed_ = workInBehavior_.speed_;
		Vector3 velo = {direction_.x * playerSpeed_,0.0f,direction_.y * playerSpeed_};
		velo *=  Time::DeltaTime();
		host_->SetPosition(host_->GetPosition() + velo);
	}

	// 回避
	if(Input::ReleaseKey(KeyCode::LShift)){
		host_->TransitionBehavior(std::make_unique<PlayerAvoidanceBehavior>(host_,lastDir_));
		return;
	}
}