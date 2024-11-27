#include "PlayerBehaviorManager.h"

#include "../Player.h"

PlayerBehaviorManager::PlayerBehaviorManager(Player* _player)
	: pPlayer_(_player) {

}

PlayerBehaviorManager::~PlayerBehaviorManager() {}

void PlayerBehaviorManager::Initialize() {}

void PlayerBehaviorManager::Update() {
	if(currentBehavior_) {
		currentBehavior_->Update();

	}
}



void PlayerBehaviorManager::AddBehavior(const std::string& _name, BasePlayerBehavior* _addBehavior) {
	std::unique_ptr<BasePlayerBehavior> add(_addBehavior);
	behaviorMap_[_name] = std::move(add);
}
