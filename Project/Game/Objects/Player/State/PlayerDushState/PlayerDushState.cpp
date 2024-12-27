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

	dushStartSpeed_     = 20.0f;
	dushNormalSpeed_    = 18.0f;

	dushTransitionTime_ = 0.2f;
	currentTime_        = 0.0f;

	isAccelerateEnded_  = false;

}

void PlayerDushState::Update() {

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


void PlayerDushState::MoveUpdate() {
	Transform* playerTransform = pPlayer_->GetTransform();

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / dushTransitionTime_, 0.0f, 1.0f);
	float speed = std::lerp(
		dushStartSpeed_, dushNormalSpeed_,
		lerpT
	);

	Vec3 velocity = pPlayer_->GetDirection() * speed * Time::DeltaTime();
	velocity.y    = pPlayer_->GetVelocity().y;
	pPlayer_->SetVelocity(velocity);
	playerTransform->position += pPlayer_->GetVelocity();


	if(lerpT == 1.0f) {
		isAccelerateEnded_ = true;
	}
}