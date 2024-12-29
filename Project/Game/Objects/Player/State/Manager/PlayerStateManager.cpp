#include "PlayerStateManager.h"

#include "../PlayerDushState/PlayerDushState.h"
#include "../PlayerJumpState/PlayerJumpState.h"
#include "../PlayerRootState/PlayerRootState.h"
#include "../PlayerNormalAttack/PlayerNormalAttack.h"


PlayerStateManager::PlayerStateManager(Player* _playerPtr) 
	: pPlayer_(_playerPtr) {

}
PlayerStateManager::~PlayerStateManager() {}

void PlayerStateManager::Initialize() {

	/// enumの順番にステートを追加
	states_.push_back(std::make_unique<PlayerRootState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerJumpState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerDushState>(pPlayer_, this));
	states_.push_back(std::make_unique<PlayerNormalAttack>(pPlayer_, this));
	
	currentState_ = states_[PlayerStateOrder_Root].get();
	currentState_->Start();

}

void PlayerStateManager::Update() {
	

	if(currentState_) {
		currentState_->Update();

		/// currentのステートが終了したら次のステートに移行
		if(currentState_->IsEnd()) {

			/// 前のステートを記憶
			beforeState_ = static_cast<int>(std::distance(states_.begin(), std::find_if(states_.begin(), states_.end(), [&](const std::unique_ptr<IPlayerState>& _state) {
				return _state.get() == currentState_;
			})));

			currentState_->Exit();
			currentState_ = states_[currentState_->NextStateIndex()].get();
			currentState_->Start();
		}

	}


}
