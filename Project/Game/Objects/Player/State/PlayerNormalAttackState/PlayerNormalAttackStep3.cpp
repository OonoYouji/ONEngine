#include "PlayerNormalAttackStep3.h"

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"
#include "../../PlayerAttackCollider/PlayerAttackCollider.h"

PlayerNormalAttackStep3::PlayerNormalAttackStep3(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

}

PlayerNormalAttackStep3::~PlayerNormalAttackStep3() {}


void PlayerNormalAttackStep3::Start() {

	maxTime_     = 0.2f;
	currentTime_ = 0.0f;
	isEnd_       = false;
}

void PlayerNormalAttackStep3::Update() {
	currentTime_ += Time::DeltaTime();
	if(currentTime_ >= maxTime_) {
		isEnd_ = true;
	}
}

void PlayerNormalAttackStep3::Exit() {
	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetColliderActive(false);
}


bool PlayerNormalAttackStep3::IsEnd() {
	return isEnd_;
}

int PlayerNormalAttackStep3::NextStateIndex() {
	return PlayerStateOrder_Root;
}

void PlayerNormalAttackStep3::Debug() {

}

void PlayerNormalAttackStep3::Save() {}
void PlayerNormalAttackStep3::Load() {}