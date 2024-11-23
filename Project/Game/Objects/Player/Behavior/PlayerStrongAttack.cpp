#define NOMINMAX
#include "PlayerStrongAttack.h"

/// std
#include <algorithm>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Input/Input.h"

#include "../Player.h"
#include "../Collision/PlayerAttackCollider/PlayerAttackCollider.h"
#include "PlayerRootBehavior.h"


PlayerStrongAttack::PlayerStrongAttack(Player* _player, int _phase) : IPlayerBehavior(_player) {
	phase_ = _phase;
	nextBehavior_ = ROOT;

	currentTime_ = 0.0f;
	maxTime_     = 2.0f; /// 仮の値 ioで設定させる

	host_->SetAnimationModel(
		"Player_StrongAttack_5_P",
		"Player_StrongAttack_5_W"
	);

	host_->SetAnimationTotalTime(maxTime_);
}

void PlayerStrongAttack::Update() {

	currentTime_ += Time::DeltaTime();

	host_->GetAttackCollider()->isActive = true;

	if(currentTime_ >= maxTime_) {
		host_->GetAttackCollider()->isActive = false;
		host_->TransitionBehavior(std::make_unique<PlayerRootBehavior>(host_));
		return;
	}

}





