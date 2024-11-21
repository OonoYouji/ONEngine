#include "../Player.h"
#include "PlayerRootBehavior.h"

#include "PlayerStrongAttack.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerWeakAttack.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"
#include "Library/Math/LerpShortAngle.h"

#include "Objects/Camera/GameCamera.h"

PlayerRootBehavior::PlayerRootBehavior(Player* _host) :IPlayerBehavior(_host), workInBehavior_(host_->GetWorkRootBehavior()) {

	/// プレイヤーが持つ最後に向いていた方向を取得
	lastDir_ = _host->GetLastDirection();
}

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

	
	{	// Rotate Update
		if(direction_.x != 0 || direction_.y != 0) {
			lastDir_ = direction_;
			host_->SetLastDirection(lastDir_);
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
			host_->SetAnimationModel("Player_Walk");
			host_->SetAnimationFlags(0, false);

		} else {
			host_->SetAnimationModel("Player_Wait");
			host_->SetAnimationFlags(0, false);

		}

	}

	InputNextBehavior();
}

void PlayerRootBehavior::InputNextBehavior() {

	bool isWeakAttack   = false;
	bool isStrongAttack = false;
	bool isDush         = false;

	/// 回避のための入力
	isDush         |= Input::TriggerKey(KeyCode::Space);
	isDush         |= Input::TriggerPadRT();
	isDush         |= Input::TriggerPadButton(PadCode::RightShoulder);

	/// 次の弱攻撃のための入力
	isWeakAttack   |= Input::TriggerKey(KeyCode::J);
	isWeakAttack   |= Input::TriggerPadButton(PadCode::B);

	/// 次の強攻撃のための入力
	isStrongAttack |= Input::TriggerKey(KeyCode::K);
	isStrongAttack |= Input::TriggerPadButton(PadCode::A);


	/// 回避
	if(isDush) {
		host_->TransitionBehavior(std::make_unique<PlayerAvoidanceBehavior>(host_));
		return;
	} 
	
	/// 弱攻撃
	if(isWeakAttack) {
		host_->TransitionBehavior(std::make_unique<PlayerWeakAttack>(host_, 0));
		return;
	}
	
	/// 強攻撃
	if(isStrongAttack) {
		host_->TransitionBehavior(std::make_unique<PlayerStrongAttack>(host_));
		return;
	}
}