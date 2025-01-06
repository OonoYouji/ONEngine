#include "IPlayerState.h"

/// external
#include <imgui.h>

IPlayerState::IPlayerState(Player* _player, PlayerStateManager* _stateManager)
	: pPlayer_(_player), pStateManager_(_stateManager) {

}
IPlayerState::~IPlayerState() {}

void IPlayerState::Debug() {
	ImGui::Text("empty...");
}

