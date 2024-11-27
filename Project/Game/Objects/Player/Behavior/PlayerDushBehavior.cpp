#include "PlayerDushBehavior.h"

PlayerDushBehavior::PlayerDushBehavior(Player* _player)
	: BasePlayerBehavior("DushBehavior", _player) {
	AddVariables();
}

PlayerDushBehavior::~PlayerDushBehavior() {}

void PlayerDushBehavior::Start() {

}

void PlayerDushBehavior::Update() {

}

void PlayerDushBehavior::Exit() {

}

std::string PlayerDushBehavior::GetNextBehavior() {

	return std::string();
}

bool PlayerDushBehavior::CanExit() {
	return false;
}
