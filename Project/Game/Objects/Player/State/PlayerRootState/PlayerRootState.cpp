#include "PlayerRootState.h"

#include "../../Player.h"

PlayerRootState::PlayerRootState(Player* _player) : IPlayerState(_player) {}
PlayerRootState::~PlayerRootState() {}

void PlayerRootState::Start() {

}

void PlayerRootState::Update() {

	Transform* playerTransform = pPlayer_->GetTransform();
	playerTransform->position += pPlayer_->GetDirection() * pPlayer_->GetMoveSpeed();

}

void PlayerRootState::Exit() {

}

bool PlayerRootState::IsEnd() {
	return false;
}

int PlayerRootState::NextStateIndex() {
	return 0;
}
