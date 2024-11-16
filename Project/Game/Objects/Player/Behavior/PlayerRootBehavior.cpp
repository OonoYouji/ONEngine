#include "../Player.h"
#include "PlayerRootBehavior.h"

#include "PlayerAvoidanceBehavior.h"
#include "PlayerWeakAttack.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"
#include "Library/Math/LerpShortAngle.h"

#include "Objects/Camera/GameCamera.h"

PlayerRootBehavior::PlayerRootBehavior(Player* _host) :IPlayerBehavior(_host), workInBehavior_(host_->GetWorkRootBehavior()) {}

void PlayerRootBehavior::Update() {
	direction_ = {
		static_cast<float>(Input::PressKey(KeyCode::D)) - static_cast<float>(Input::PressKey(KeyCode::A)),
		static_cast<float>(Input::PressKey(KeyCode::W)) - static_cast<float>(Input::PressKey(KeyCode::S))
	};
	direction_ += Input::GetLeftStick();

	direction_ = direction_.Normalize();

	auto GetYawFromQuaternion = [](const Quaternion& q) {
		return std::atan2(
			2.0f * (q.y * q.w + q.x * q.z),
			1.0f - 2.0f * (q.x * q.x + q.y * q.y)
		);
	};

	{	/// 方向をカメラに合わせる
		Mat4 matCameraRotateY = Mat4::MakeRotateY(GetYawFromQuaternion(host_->GetCamera()->GetQuaternion()));
		Vec3 dir = Mat4::TransformNormal({direction_.x, 0.0f, direction_.y}, matCameraRotateY);
		direction_.x = dir.x;
		direction_.y = dir.z;
	}

	// Rotate Update
	{
		if(direction_.x != 0 || direction_.y != 0) {
			lastDir_ = direction_;
		}
		Vector3 rotate = host_->GetRotate();
		rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDir_.x, lastDir_.y), workInBehavior_.rotateLerpSensitivity_);
		host_->SetRotate(rotate);
	}

	{  // Postion Update
		float playerSpeed_ = workInBehavior_.speed_;
		Vec3  velocity     = {
			direction_.x, 0.0f, direction_.y
		};

		velocity *= playerSpeed_ * Time::DeltaTime();

		host_->SetPosition(host_->GetPosition() + velocity);


		/// 移動しているか確認、アニメーションを変える
		if(velocity != Vec3(0, 0, 0)) {
			host_->SetAnimationModel("KariPlayer_Walk");
		} else {
			host_->SetAnimationModel("KariPlayer_Wait");
		}

	}

	InputNextBehavior();
}

void PlayerRootBehavior::InputNextBehavior() {
	// 回避
	if(Input::ReleaseKey(KeyCode::LShift)) {
		host_->TransitionBehavior(std::make_unique<PlayerAvoidanceBehavior>(host_));
		return;
	} else if(Input::ReleaseKey(KeyCode::J)) {
		host_->TransitionBehavior(std::make_unique<PlayerWeakAttack>(host_, 0));
		return;
	}
}