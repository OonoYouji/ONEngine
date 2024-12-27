#include "PlayerStateManager.h"

#include "../PlayerDushState/PlayerDushState.h"
#include "../PlayerJumpState/PlayerJumpState.h"
#include "../PlayerRootState/PlayerRootState.h"


PlayerStateManager::PlayerStateManager(Player* _playerPtr) 
	: pPlayer_(_playerPtr) {

}
PlayerStateManager::~PlayerStateManager() {}

void PlayerStateManager::Initialize() {

	/// enumの順番にステートを追加
	states_.push_back(std::make_unique<PlayerRootState>(pPlayer_));
	states_.push_back(std::make_unique<PlayerJumpState>(pPlayer_));
	states_.push_back(std::make_unique<PlayerDushState>(pPlayer_));
	
	currentState_ = states_[PlayerStateOrder_Root].get();
	currentState_->Start();

}

void PlayerStateManager::Update() {
	

	if(currentState_) {
		currentState_->Update();

		/// currentのステートが終了したら次のステートに移行
		if(currentState_->IsEnd()) {
			currentState_->Exit();
			currentState_ = states_[currentState_->NextStateIndex()].get();
			currentState_->Start();
		}

	}


}
