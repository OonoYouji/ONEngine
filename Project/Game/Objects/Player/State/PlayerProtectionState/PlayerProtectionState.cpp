#include "PlayerProtectionState.h"

/// external
#include <imgui.h>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"


PlayerProtectionState::PlayerProtectionState(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

}

PlayerProtectionState::~PlayerProtectionState() {}

void PlayerProtectionState::Start() {
	time_ = 0.0f;
}

void PlayerProtectionState::Update() {
	time_ += Time::DeltaTime();

}

void PlayerProtectionState::Exit() {

}

bool PlayerProtectionState::IsEnd() {

	/// 入力がなくなったら終了
	if(!pPlayer_->GetFlag(PlayerFlag_IsProtection).Stay()) {
		return true;
	}

	return false;
}

int PlayerProtectionState::NextStateIndex() {
	return 0;
}


void PlayerProtectionState::Debug() {
	ImGui::Text("time : %f", time_);
}

void PlayerProtectionState::Save() {


}

void PlayerProtectionState::Load() {


}