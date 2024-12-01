#include "PlayerDushBehavior.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"

/// game
#include "../Player.h"

PlayerDushBehavior::PlayerDushBehavior(Player* _player)
	: BasePlayerBehavior("DushBehavior", _player) {
	AddVariables();
}

PlayerDushBehavior::~PlayerDushBehavior() {}

void PlayerDushBehavior::Start() {

	ApplyVariables();

	dushVelocity_ = player_->GetCommonData().velocity.Normalize();
	currentTime_  = maxTime_;
}

void PlayerDushBehavior::Update() {
	currentTime_ -= Time::DeltaTime();

	dushVelocity_ = dushVelocity_.Normalize() * dushSpeed_;

	player_->SetPosition(player_->GetPosition() + dushVelocity_);
}

void PlayerDushBehavior::Exit() {
	currentTime_ = 0.0f;
}

std::string PlayerDushBehavior::GetNextBehavior() {
	return "root";
}

bool PlayerDushBehavior::CanExit() {
	return currentTime_ < 0.0f;
}

void PlayerDushBehavior::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();

	vm->AddValue(kName_, "maxTime",   maxTime_);
	vm->AddValue(kName_, "dushSpeed", dushSpeed_);

}

void PlayerDushBehavior::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	maxTime_   = vm->GetValue<float>(kName_, "maxTime");
	dushSpeed_ = vm->GetValue<float>(kName_, "dushSpeed");

}
