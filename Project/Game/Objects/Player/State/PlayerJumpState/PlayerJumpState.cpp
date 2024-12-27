#include "PlayerJumpState.h"

#include "../PlayerRootState/PlayerRootState.h"


PlayerJumpState::PlayerJumpState(Player* _player) : IPlayerState(_player) {}
PlayerJumpState::~PlayerJumpState() {}

void PlayerJumpState::Start() {

}

void PlayerJumpState::Update() {

}

void PlayerJumpState::Exit() {

}

bool PlayerJumpState::IsEnd() {
	return false;
}

int PlayerJumpState::NextStateIndex() {
	return 0;
}
