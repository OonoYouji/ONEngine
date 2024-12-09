#include "PlayerRootBehavior.h"

/// engine
#include "FrameManager/Time.h"

/// game
#include "../Player.h"
#include "Objects/Camera/GameCamera.h"

PlayerRootBehavior::PlayerRootBehavior(Player* _player) 
	: BasePlayerBehavior("RootBehavior", _player) {

}

PlayerRootBehavior::~PlayerRootBehavior() {}

void PlayerRootBehavior::Start() {

	

}

void PlayerRootBehavior::Update() {
	CommonData& data = player_->GetCommonData();

	Vec2 inputLeftStick = Input::GetLeftStick();
	inputLeftStick = inputLeftStick.Normalize();
	data.velocity = {
		inputLeftStick.x * data.movementSpeed * Time::DeltaTime(),
		0.0f,
		inputLeftStick.y * data.movementSpeed * Time::DeltaTime(),
	};

	auto GetYawFromQuaternion = [](const Quaternion& q) {
		return std::atan2(
			2.0f * (q.y * q.w + q.x * q.z),
			1.0f - 2.0f * (q.x * q.x + q.y * q.y)
		);
	};

	Mat4 matCameraRotateY = Mat4::MakeRotateY(GetYawFromQuaternion(player_->GetGameCamera()->GetQuaternion()));
	data.velocity = Mat4::TransformNormal(data.velocity, matCameraRotateY);

	player_->GetCommonData().position += data.velocity;
}

void PlayerRootBehavior::Exit() {

}

std::string PlayerRootBehavior::GetNextBehavior() {
	if(Input::TriggerPadButton(PadCode::RightShoulder)) { return "dush"; }
	return std::string();
}

bool PlayerRootBehavior::CanExit() {
	return true;
}
