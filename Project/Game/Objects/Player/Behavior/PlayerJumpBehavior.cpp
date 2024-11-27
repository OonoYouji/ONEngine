#include "PlayerJumpBehavior.h"

PlayerJumpBehavior::PlayerJumpBehavior(Player* _player) 
	: BasePlayerBehavior("JumpBehavior", _player) {

	AddVariables();
}

PlayerJumpBehavior::~PlayerJumpBehavior() {}



void PlayerJumpBehavior::Start() {

	ApplyVariables();
}

void PlayerJumpBehavior::Update() {

}

void PlayerJumpBehavior::Exit() {

}

std::string PlayerJumpBehavior::GetNextBehavior() {
	if(Input::PressPadButton(PadCode::RightShoulder)) { return "dush"; }
	return "root";
}

bool PlayerJumpBehavior::CanExit() {
	if(currentTime_ >= maxTime_) {
		return true;
	}
	return false;
}




void PlayerJumpBehavior::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	vm->AddValue(kName_, "maxHeight", maxHeight_);
	vm->AddValue(kName_, "maxTime", maxTime_);
}

void PlayerJumpBehavior::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	maxHeight_ = vm->GetValue<float>(kName_, "maxHeight");
	maxTime_   = vm->GetValue<float>(kName_, "maxTime");
}
