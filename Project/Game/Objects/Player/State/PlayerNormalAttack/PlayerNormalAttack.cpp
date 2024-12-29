#include "PlayerNormalAttack.h"

PlayerNormalAttack::PlayerNormalAttack(Player* _player, PlayerStateManager* _stateManager) : IPlayerState(_player, _stateManager) {}
PlayerNormalAttack::~PlayerNormalAttack() {}

void PlayerNormalAttack::Start() {

}

void PlayerNormalAttack::Update() {

}

void PlayerNormalAttack::Exit() {

}

bool PlayerNormalAttack::IsEnd() {
	return false;
}

int PlayerNormalAttack::NextStateIndex() {
	return 0;
}

