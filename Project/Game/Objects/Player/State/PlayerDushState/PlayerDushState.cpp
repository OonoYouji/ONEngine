#include "PlayerDushState.h"

/// std
#include <algorithm>

// engine
#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"

/// user
#include "../../Player.h"
#include "../PlayerRootState/PlayerRootState.h"

PlayerDushState::PlayerDushState(Player* _player, PlayerStateManager* _stateManager) : IPlayerState(_player, _stateManager) {}
PlayerDushState::~PlayerDushState() {}

void PlayerDushState::Start() {

	dushStartSpeed_     = 40.0f;
	dushNormalSpeed_    = 30.0f;

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

	/// 加速が終わった通常のダッシュ状態になったら終了
	if(isAccelerateEnded_) {
		if(!pPlayer_->GetFlag(PlayerFlag_IsDush).Stay()) {
			return true;
		}
	}
	
	if(pPlayer_->GetFlag(PlayerFlag_IsJump).Enter()) {
		return true;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsAttack).Enter()) {
		return true;
	}

	return false;
}

int PlayerDushState::NextStateIndex() {
	if(!pPlayer_->GetFlag(PlayerFlag_IsDush).Stay()) {
		return PlayerStateOrder_Root;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsJump).Enter()) {
		return PlayerStateOrder_Jump;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsAttack).Enter()) {
		return PlayerStateOrder_NormalAttack;
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