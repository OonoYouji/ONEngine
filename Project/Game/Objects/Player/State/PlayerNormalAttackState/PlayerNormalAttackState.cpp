#include "PlayerNormalAttackState.h"

PlayerNormalAttackState::PlayerNormalAttackState(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

}

PlayerNormalAttackState::~PlayerNormalAttackState() {}


void PlayerNormalAttackState::Start() {

}

void PlayerNormalAttackState::Update() {

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
