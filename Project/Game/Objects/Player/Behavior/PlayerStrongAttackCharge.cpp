#define NOMINMAX
#include "PlayerStrongAttackCharge.h"

/// std
#include <algorithm>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Input/Input.h"

#include "../Player.h"
#include "PlayerRootBehavior.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerStrongAttack.h"

PlayerStrongAttackCharge::PlayerStrongAttackCharge(Player* _player, int _phase, float _nextChargeTime) : IPlayerBehavior(_player) {

	nextBehavior_ = ROOT;

	currentTime_    = 0.0f;
	currentPhase_   = _phase;
	nextChargeTime_ = _nextChargeTime;

	if(currentPhase_ == THIRD) {
		isChargeMax_ = true;
	}


	/// チャージ段階が1以上になったらダメージ量を設定する
	if(currentPhase_ != NONE) {

		const Player::StrongAttackBehavior& strongAttack = host_->GetStrongAttackBehavior();
		host_->SetDamage(strongAttack.damages_[currentPhase_ - 1]);
	}

	const std::string animationFilePath = "Player_StrongAttack_" + std::to_string(currentPhase_ + 1);
	host_->SetAnimationModel(
		animationFilePath + "_P",
		animationFilePath + "_W"
	);

	host_->SetIsActiveWeapon(true);
	if(currentPhase_ == NONE) {
		host_->SetAnimationTotalTime(1.0f);
	} else {
		host_->SetAnimationTotalTime(0.5f);
	}
	host_->SetAnimationFlags(0);


	if(currentPhase_ != THIRD) {	/// 値のio
		VariableManager* vm = VariableManager::GetInstance();
		const std::string groupName = "StrongAttackBehavior";
		nextTime_ = vm->GetValue<float>(groupName, "nextChargeTime" + std::to_string(currentPhase_));
	}


}


void PlayerStrongAttackCharge::Update() {
	currentTime_ += Time::DeltaTime();



	isFinish_ = false;
	isFinish_ |= Input::ReleaseKey(KeyCode::K);
	isFinish_ |= Input::ReleasePadButton(PadCode::A);

	/// 入力をやめた瞬間が攻撃する瞬間
	if(isFinish_) {
		if(currentPhase_ != NONE) {
			nextBehavior_ = STRONG_ATTACK;
		} else {
			nextBehavior_ = ROOT;
		}
	}


	if(currentTime_ >= nextChargeTime_) {
		if(currentPhase_ != THIRD) {

			isFinish_ = true;
			nextBehavior_ = NEXT_CHARGE;
		}
	}


	if(isFinish_) {
		std::unique_ptr<IPlayerBehavior> nextBehavior;
		switch(nextBehavior_) {
		case ROOT:
			nextBehavior.reset(new PlayerRootBehavior(host_));
			break;
		case NEXT_CHARGE:
			nextBehavior.reset(new PlayerStrongAttackCharge(host_, currentPhase_ + 1, 1.0f));
			break;
		case STRONG_ATTACK:
			nextBehavior.reset(new PlayerStrongAttack(host_, currentPhase_));
			break;
		case AVOIDANCE:
			nextBehavior.reset(new PlayerAvoidanceBehavior(host_));
			break;
		}

		host_->TransitionBehavior(std::move(nextBehavior));
	}

}





