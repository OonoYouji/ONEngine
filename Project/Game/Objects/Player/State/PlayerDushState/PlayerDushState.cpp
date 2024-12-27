#include "PlayerDushState.h"

/// std
#include <algorithm>

// engine
#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"

/// user
#include "../../Player.h"
#include "../PlayerRootState/PlayerRootState.h"

PlayerDushState::PlayerDushState(Player* _player) : IPlayerState(_player) {}
PlayerDushState::~PlayerDushState() {}

void PlayerDushState::Start() {

	dushStartSpeed_     = 1.0f;
	dushNormalSpeed_    = 0.2f;

	dushTransitionTime_ = 0.2f;
	currentTime_        = 0.0f;

	isAccelerateEnded_  = false;

}

void PlayerDushState::Update() {

	RotateUpdate();
	MoveUpdate();

}

void PlayerDushState::Exit() {

}

bool PlayerDushState::IsEnd() {
	if(isAccelerateEnded_) {
		return !pPlayer_->GetFlag(PlayerFlag_IsDush).Stay();
	}

	return false;
}

int PlayerDushState::NextStateIndex() {
	if(!pPlayer_->GetFlag(PlayerFlag_IsDush).Stay()) {
		return PlayerStateOrder_Root;
	}

	return 0;
}


void PlayerDushState::RotateUpdate() {
	const Vec3& direction = pPlayer_->GetDirection();
	const Vec3& lastDir = pPlayer_->GetLastDirection();

	if(direction.x != 0 || direction.z != 0) {
		pPlayer_->SetLastDirection(direction);
	}

	Vector3 rotate = pPlayer_->GetMesh()->GetRotate();
	rotate.y = LerpShortAngle(rotate.y, std::atan2(lastDir.x, lastDir.z), 0.1f);
	pPlayer_->SetMeshRotate(rotate);
}

void PlayerDushState::MoveUpdate() {
	Transform* playerTransform = pPlayer_->GetTransform();

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / dushTransitionTime_, 0.0f, 1.0f);
	float speed = std::lerp(
		dushStartSpeed_, dushNormalSpeed_,
		lerpT
	);

	pPlayer_->SetVelocity(pPlayer_->GetDirection() * speed);
	playerTransform->position += pPlayer_->GetVelocity();


	if(lerpT == 1.0f) {
		isAccelerateEnded_ = true;
	}
}