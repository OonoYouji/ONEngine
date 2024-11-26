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

	VariableManager* vm = VariableManager::GetInstance();
	maxTime_     = vm->GetValue<float>("StrongAttackBehavior", "actionTime"); /// 仮の値 ioで設定させる

	host_->SetAnimationModel(
		"Player_StrongAttack_5_P",
		"Player_StrongAttack_5_W"
	);

	host_->SetAnimationTotalTime(maxTime_);

	host_->SetAttackMode(PlayerAttackCollider::MODE_STRONG_ATTACK);

	host_->PlayAudio("strongAttack" + std::to_string(_phase) + ".wav", 0.5f);

	host_->SetNextStrongChargeCount(0);

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





