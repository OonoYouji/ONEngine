#include "PlayerDushState.h"

#include "../PlayerRootState/PlayerRootState.h"

PlayerDushState::PlayerDushState(Player* _player) : IPlayerState(_player) {}
PlayerDushState::~PlayerDushState() {}

void PlayerDushState::Start() {

}

void PlayerDushState::Update() {

}

void PlayerDushState::Exit() {

}

bool PlayerDushState::IsEnd() {
	return false;
}

int PlayerDushState::NextStateIndex() {
	return 0;
}
