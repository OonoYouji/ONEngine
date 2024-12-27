#include "PlayerJumpState.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"
#include "../PlayerRootState/PlayerRootState.h"


PlayerJumpState::PlayerJumpState(Player* _player) : IPlayerState(_player) {}
PlayerJumpState::~PlayerJumpState() {}

void PlayerJumpState::Start() {

	playerTransform_ = pPlayer_->GetTransform(); 
	
	VelocitySetting();

	inputTimeLimit_ = 0.2f;

}

void PlayerJumpState::Update() {

	if(inputTimeLimit_ > 0.0f) {
		inputTimeLimit_ -= Time::DeltaTime();

		if(pPlayer_->GetFlag(PlayerFlag_IsJump).Stay()) {
			VelocitySetting();
		}
	}

	playerTransform_->position += pPlayer_->GetVelocity();
	
}

void PlayerJumpState::Exit() {

}

bool PlayerJumpState::IsEnd() {
	if(playerTransform_->position.y < 0.0f) {
		return true;
	}

	return false;
}

int PlayerJumpState::NextStateIndex() {
	if(playerTransform_->position.y < 0.0f) {
		return PlayerStateOrder_Root;
	}

	return 0;
}



void PlayerJumpState::VelocitySetting() {
	Vec3 velocity = pPlayer_->GetVelocity();
	velocity.y    = 10.0f * Time::DeltaTime();
	pPlayer_->SetVelocity(velocity);
}
