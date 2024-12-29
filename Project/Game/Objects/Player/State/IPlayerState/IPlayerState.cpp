#include "IPlayerState.h"

IPlayerState::IPlayerState(Player* _player, PlayerStateManager* _stateManager)
	: pPlayer_(_player), pStateManager_(_stateManager) {

}
IPlayerState::~IPlayerState() {}

