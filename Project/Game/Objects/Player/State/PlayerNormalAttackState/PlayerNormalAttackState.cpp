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

	direction_ = pPlayer_->GetLastDirection();
	if(direction_.Len() == 0.0f) {
		direction_ = Vec3(0,0,1);
	}

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
	colliderSize_     = { 1.5f, 1.5f, 5.0f };
	colliderRotate_.y = std::atan2(direction_.x, direction_.z);

	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetPosition(colliderPosition_);
	collider->SetRotate(colliderRotate_);
	collider->SetScale(colliderSize_);
	collider->UpdateMatrix();
	collider->SetColliderActive(true);

}

void PlayerNormalAttackState::Update() {

	currentTime_ += Time::DeltaTime();
	float lerpT = std::clamp(currentTime_ / maxTime_, 0.0f, 1.0f);

	pPlayer_->SetPosition(
		startPosition_ + (direction_ * 5.0f) * lerpT
	);

}

void PlayerNormalAttackState::Exit() {
	PlayerAttackCollider* collider = pPlayer_->GetAttackCollider();
	collider->SetColliderActive(false);
}


bool PlayerNormalAttackState::IsEnd() { 
	return currentTime_ >= maxTime_; 
}

int PlayerNormalAttackState::NextStateIndex() { 
	return PlayerStateOrder_Root; 
}


void PlayerNormalAttackState::Debug() {

}
