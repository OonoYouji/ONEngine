#include "PlayerStateManager.h"

/// std

/// external
#include <imgui.h>

/// user
#include "../PlayerDushState/PlayerDushState.h"
#include "../PlayerJumpState/PlayerJumpState.h"
#include "../PlayerRootState/PlayerRootState.h"
#include "../PlayerProtectionState/PlayerProtectionState.h"


PlayerStateManager::PlayerStateManager(Player* _playerPtr) 
	: pPlayer_(_playerPtr) {

}
PlayerStateManager::~PlayerStateManager() {}

void PlayerStateManager::Initialize() {

	/// enumの順番にステートを追加
	states_.push_back(std::make_unique<PlayerRootState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerJumpState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerDushState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerProtectionState>(pPlayer_, this));
	
	currentState_ = states_[PlayerStateOrder_Root].get();
	currentState_->Start();

}

void PlayerStateManager::Update() {
	

	if(currentState_) {
		currentState_->Update();

		/// currentのステートが終了したら次のステートに移行
		if(currentState_->IsEnd()) {

			currentState_->Exit();
			
			currentStateIndex_ = currentState_->NextStateIndex();

			/// 前のステートを記憶
			beforeState_ = static_cast<int>(std::distance(states_.begin(), std::find_if(states_.begin(), states_.end(), [&](const std::unique_ptr<IPlayerState>& _state) {
				return _state.get() == currentState_;
			})));

			currentState_ = states_[currentStateIndex_].get();


			currentState_->Start();

		}

	}

	 
}

void PlayerStateManager::Debug() {

	if(ImGui::BeginChild("state list", ImVec2(0.0f, 320.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
		for(auto& state : states_) {
			state->Debug();
		}
		ImGui::EndChild();
	}

}
