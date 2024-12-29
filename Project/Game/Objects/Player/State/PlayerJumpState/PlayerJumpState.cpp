#include "PlayerJumpState.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"
#include "Debugger/Assertion.h"

/// user
#include "../../Player.h"
#include "../PlayerRootState/PlayerRootState.h"


PlayerJumpState::PlayerJumpState(Player* _player) : IPlayerState(_player) {}
PlayerJumpState::~PlayerJumpState() {}

void PlayerJumpState::Start() {

	playerTransform_ = pPlayer_->GetTransform(); 
	
	VelocitySetting();

	inputTimeLimit_ = 0.2f;
	canDoubleJump_  = true;
}

void PlayerJumpState::Update() {

	if(inputTimeLimit_ > 0.0f) {
		inputTimeLimit_ -= Time::DeltaTime();

		if(pPlayer_->GetFlag(PlayerFlag_IsJump).Stay()) {
			VelocitySetting();
		} else if(pPlayer_->GetFlag(PlayerFlag_IsJump).Exit()) {
			inputTimeLimit_ = 0.0f;
		}

	}


	/// 二段ジャンプの処理
	if(canDoubleJump_) {
		if(pPlayer_->GetFlag(PlayerFlag_IsJump).Enter()) {
			inputTimeLimit_ = 0.2f;
			canDoubleJump_  = false;
			VelocitySetting();
		}
	}


	/// 向きの修正を行う
	Vec3 nextVelocity    = pPlayer_->GetDirection() * Time::DeltaTime();
	nextVelocity.y       = 0.0f;
	
	Vec3 currentVelocity = pPlayer_->GetVelocity();
	currentVelocity.y    = 0.0f;

	Vec3 velocity = Vec3::Lerp(currentVelocity.Normalize(), nextVelocity.Normalize(), 0.5f);
	velocity = velocity.Normalize() * currentVelocity.Len();
	velocity.y = pPlayer_->GetVelocity().y;

	pPlayer_->SetVelocity(velocity);

	playerTransform_->position += pPlayer_->GetVelocity();

	//Assert(!std::isinf(playerTransform_->position.x), "is inf");
	//Assert(!std::isinf(playerTransform_->position.y), "is inf");
	//Assert(!std::isinf(playerTransform_->position.z), "is inf");

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
