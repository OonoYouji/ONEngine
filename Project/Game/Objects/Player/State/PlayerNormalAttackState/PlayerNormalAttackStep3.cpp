#include "PlayerNormalAttackStep3.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"
#include "Math/Easing.h"

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

	float lerpT = std::clamp(currentTime_ / maxTime_, 0.0f, 1.0f);

	pPlayer_->GetTransform()->position.y = std::lerp(
		0.0f, 10.0f,
		Ease::InOut::Quad(lerpT)
	);

	if(lerpT == 1.0f) {
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
	return PlayerStateOrder_BulletAttack;
}

void PlayerNormalAttackStep3::Debug() {

}

void PlayerNormalAttackStep3::Save() {}
void PlayerNormalAttackStep3::Load() {}
