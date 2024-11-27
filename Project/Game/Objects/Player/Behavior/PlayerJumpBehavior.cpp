#include "PlayerJumpBehavior.h"

PlayerJumpBehavior::PlayerJumpBehavior(Player* _player) 
	: BasePlayerBehavior("JumpBehavior", _player) {
}

PlayerJumpBehavior::~PlayerJumpBehavior() {}



void PlayerJumpBehavior::Start() {

}

void PlayerJumpBehavior::Update() {

}

void PlayerJumpBehavior::Exit() {

}

std::string PlayerJumpBehavior::GetNextBehavior() {
	return std::string();
}

bool PlayerJumpBehavior::CanExit() {
	return false;
}

void PlayerJumpBehavior::AddVariables() {
}

void PlayerJumpBehavior::ApplyVariables() {}
