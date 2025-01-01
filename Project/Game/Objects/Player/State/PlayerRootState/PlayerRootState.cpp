#include "PlayerRootState.h"

/// engine
#include "FrameManager/Time.h"
#include "Math/LerpShortAngle.h"

/// user
#include "../../Player.h"

PlayerRootState::PlayerRootState(Player* _player, PlayerStateManager* _stateManager) : IPlayerState(_player, _stateManager) {}
PlayerRootState::~PlayerRootState() {}

void PlayerRootState::Start() {

}

void PlayerRootState::Update() {

	/// 座標の更新
	MoveUpdate();

}

void PlayerRootState::Exit() {

}

bool PlayerRootState::IsEnd() {
	if(pPlayer_->GetFlag(PlayerFlag_IsDush).Enter()) {
		return true;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsJump).Enter()) {
		return true;
	}
	
	if(pPlayer_->GetFlag(PlayerFlag_IsProtection).Enter()) {
		return true;
	}

	return false;
}

int PlayerRootState::NextStateIndex() {
	if(pPlayer_->GetFlag(PlayerFlag_IsDush).Enter()) {
		return PlayerStateOrder_Dush;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsJump).Enter()) {
		return PlayerStateOrder_Jump;
	}

	if(pPlayer_->GetFlag(PlayerFlag_IsProtection).Enter()) {
		return PlayerStateOrder_Protection;
	}

	return PlayerStateOrder_Root;
}


void PlayerRootState::MoveUpdate() {
	Transform* playerTransform = pPlayer_->GetTransform();

	Vec3 velocity = pPlayer_->GetDirection() * pPlayer_->GetMoveSpeed() * Time::DeltaTime();
	velocity.y    = pPlayer_->GetVelocity().y;
	pPlayer_->SetVelocity(velocity);

	playerTransform->position += pPlayer_->GetVelocity();
}
