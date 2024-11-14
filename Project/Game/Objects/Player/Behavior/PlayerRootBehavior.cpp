#include "../Player.h"
#include "PlayerRootBehavior.h"

#include "PlayerAvoidanceBehavior.h"
#include "PlayerWeakAttack.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"
#include "Library/Math/LerpShortAngle.h"

#include "Objects/Camera/GameCamera.h"

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
		rotate.y = LerpShortAngle(rotate.y,atan2(lastDir_.x,lastDir_.y),workInBehavior_.rotateLerpSensitivity_);
		host_->SetRotate(rotate);
	}

	{  // Postion Update
		float playerSpeed_ = workInBehavior_.speed_;

		Vec3 velocity = {
			direction_.x, 0.0f, direction_.y
		};

		auto GetYawFromQuaternion = [](const Quaternion& q) {
			return std::atan2(
				2.0f * (q.y * q.w + q.x * q.z),
				1.0f - 2.0f * (q.x * q.x + q.y * q.y)
			);
		};

		Mat4 matCameraRotateY = Mat4::MakeRotateY(GetYawFromQuaternion(host_->GetCamera()->GetQuaternion()));
		velocity = Mat4::TransformNormal(velocity, matCameraRotateY);
		velocity *= playerSpeed_ * Time::DeltaTime();

		host_->SetPosition(host_->GetPosition() + velocity);
	}

	InputNextBehavior();
}

void PlayerRootBehavior::InputNextBehavior(){
	// 回避
	if(Input::ReleaseKey(KeyCode::LShift)){
		host_->TransitionBehavior(std::make_unique<PlayerAvoidanceBehavior>(host_));
		return;
	} else if(Input::ReleaseKey(KeyCode::J)){
		host_->TransitionBehavior(std::make_unique<PlayerWeakAttack>(host_,0));
		return;
	}
}