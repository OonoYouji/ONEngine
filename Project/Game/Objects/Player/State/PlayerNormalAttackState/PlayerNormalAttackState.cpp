#include "PlayerNormalAttackState.h"

/// std
#include <algorithm>

/// engine
#include "FrameManager/Time.h"

/// user
#include "../../Player.h"
#include "../../PlayerAttackCollider/PlayerAttackCollider.h"
#include "Objects/Enemy/Enemy.h"

PlayerNormalAttackState::PlayerNormalAttackState(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {

}

PlayerNormalAttackState::~PlayerNormalAttackState() {}


void PlayerNormalAttackState::Start() {

	direction_ = pPlayer_->GetDirection();

	maxTime_     = 0.1f;
	currentTime_ = 0.0f;

	startPosition_ = pPlayer_->GetPosition();

	/// ターゲットがいる場合はターゲットに向かって攻撃
	if(pPlayer_->GetFlag(PlayerFlag_IsTarget).Stay()) {
		direction_ = pPlayer_->GetTargetEnemy()->GetPosition() - pPlayer_->GetPosition();
		direction_ = direction_.Normalize();
	}

	/// colliderの設定
	colliderPosition_ = pPlayer_->GetPosition() + direction_ * 1.0f;
	colliderRotate_   = { 0.0f, 0.0f, 0.0f };
	colliderSize_     = { 1.0f, 1.0f, 1.0f };
	colliderRotate_.y = std::atan2(direction_.x, direction_.z);

	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetPosition(colliderPosition_);
	collider->SetRotate(colliderRotate_);
	collider->SetScale(colliderSize_);

}

void PlayerNormalAttackState::Update() {

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / maxTime_, 0.0f, 1.0f);

	pPlayer_->SetPosition(
		startPosition_ + direction_ * 1.0f * lerpT
	);

}

void PlayerNormalAttackState::Exit() {

}


bool PlayerNormalAttackState::IsEnd() { 
	return false; 
}

int PlayerNormalAttackState::NextStateIndex() { 
	return 0; 
}


void PlayerNormalAttackState::Debug() {

}
