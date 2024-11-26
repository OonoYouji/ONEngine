#define NOMINMAX
#include "PlayerStrongAttackCharge.h"

/// std
#include <algorithm>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Input/Input.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// objects
#include "../Player.h"
#include "../Effect/PlayerStrongAttackChargeEffect.h"

#include "PlayerRootBehavior.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerStrongAttack.h"

PlayerStrongAttackCharge::PlayerStrongAttackCharge(Player* _player, int _phase, float _nextChargeTime) : IPlayerBehavior(_player) {

	nextBehavior_ = ROOT;

	currentTime_ = 0.0f;
	currentPhase_ = _phase;
	nextChargeTime_ = _nextChargeTime;

	if(currentPhase_ == THIRD) {
		isChargeMax_ = true;
	}


	effect_ = host_->GetPlayerStrongAttackChargeEffect();
	effect_->SetAnimationActive(false);

	/// チャージ段階が1以上になったらダメージ量を設定する
	if(currentPhase_ != NONE) {

		const Player::StrongAttackBehavior& strongAttack = host_->GetStrongAttackBehavior();
		host_->SetDamage(strongAttack.damages_[currentPhase_ - 1]);

		effect_->SetAnimationActive(true);
		effect_->SetTimeRate(0.5f + (0.25f * currentPhase_));
	}

	const std::string animationFilePath = "Player_StrongAttack_" + std::to_string(currentPhase_ + 1);
	host_->SetAnimationModel(
		animationFilePath + "_P",
		animationFilePath + "_W"
	);

	host_->SetIsActiveWeapon(true);
	host_->SetAnimationTotalTime(nextChargeTime_);

	if(currentPhase_ != THIRD) {
		host_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);
	} else {
		host_->SetAnimationFlags(0);
	}



	/// 音を再生する
	if(currentPhase_ != NONE) {

		if(currentPhase_ != THIRD ) {
			host_->PlayAudio("strongAttackCharge" + std::to_string(currentPhase_) + ".wav", 0.5f);
		} else {
			host_->PlayAudio("strongAttackCharge" + std::to_string(currentPhase_) + ".wav", 0.5f);
		}
	}


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
		effect_->SetAnimationActive(false);
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
		return;
	}



	/// phaseが thirdの時に効果音をリピート再生する


}





