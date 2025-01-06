#include "PlayerNormalAttackState.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"

PlayerNormalAttackState::PlayerNormalAttackState(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

}

PlayerNormalAttackState::~PlayerNormalAttackState() {}


void PlayerNormalAttackState::Start() {

	direction_ = pPlayer_->GetDirection();

	maxTime_     = 0.1f;
	currentTime_ = 0.0f;

	startPosition_ = pPlayer_->GetPosition();

	if(pPlayer_->GetFlag(PlayerFlag_IsTarget).Stay()) {

	}

}

void PlayerNormalAttackState::Update() {

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / maxTime_, 0.0f, 1.0f);



}

void PlayerNormalAttackState::Exit() {

}


bool PlayerNormalAttackState::IsEnd() { 
	return false; 
}

int PlayerNormalAttackState::NextStateIndex() { 
	return 0; 
}


void PlayerNormalAttackState::Debug() {

}
