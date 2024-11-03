#include "../Player.h"
#include "PlayerRootBehavior.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"

#include <numbers>

float lerpShortAngle(float a,float b,float t){
	constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
	constexpr float minPi = std::numbers::pi_v<float> *-2.0f;

	float diff = b - a;

	float rotate = std::fmod(diff,maxPi);

	if(rotate >= std::numbers::pi){
		rotate += minPi;
	} else if(rotate <= -std::numbers::pi){
		rotate += maxPi;
	}

	return a + rotate * t;
}

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
		rotate.y = lerpShortAngle(rotate.y,atan2(lastDir_.x,lastDir_.y),0.1f);
		host_->SetRotate(rotate);
	}

	{  // Postion Update
		float playerSpeed_ = host_->GetSpeed();
		Vector3 velo = {direction_.x * playerSpeed_,0.0f,direction_.y * playerSpeed_};
		velo *=  Time::DeltaTime();
		host_->SetPosition(host_->GetPosition() + velo);
	}
}
