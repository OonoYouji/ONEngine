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

/// this behavior
#include "PlayerRootBehavior.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerStrongAttack.h"

/// math
#include "Math/LerpShortAngle.h"


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
		animationFilePath + "_W",
		"Effect5"
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
			host_->PlayAudio("strongAttackCharge" + std::to_string(currentPhase_) + ".wav", 0.1f);
		} else {
			host_->PlayAudio("strongAttackCharge" + std::to_string(currentPhase_) + ".wav", 0.1f);
		}
	}


	{	/// 値の io

		VariableManager* vm = VariableManager::GetInstance();
		const std::string groupName = "StrongAttackBehavior";

		if(currentPhase_ != THIRD) {
			nextTime_ = vm->GetValue<float>(groupName, "nextChargeTime" + std::to_string(currentPhase_));
		} else {

			/// repeat timeをゲット
			repeatMaxTime_ = vm->GetValue<float>(groupName, "thirdSERepeatTime");
			repeatTime_ = repeatMaxTime_;
		}
	}


}


void PlayerStrongAttackCharge::Update() {
	currentTime_ += Time::DeltaTime();



	isFinish_ = false;
	isFinish_ |= Input::ReleaseKey(KeyCode::K);
	isFinish_ |= Input::ReleasePadButton(PadCode::A);

	bool isDush = false;
	isDush |= Input::TriggerKey(KeyCode::Space);
	isDush |= Input::TriggerPadRT();
	isDush |= Input::TriggerPadButton(PadCode::RightShoulder);




	{	// Rotate Update
		Vec2 lastDir = host_->GetLastDirection();
		Vector3 rotate = host_->GetRotate();

		/// 回転のスピードはここで調整
		rotate.y = LerpShortAngle(
			rotate.y, std::atan2(lastDir.x, lastDir.y),
			host_->GetWorkRootBehavior().rotateLerpSensitivity_
		);

		host_->SetRotate(rotate);
	}





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


	if(isDush) {
		if(host_->GetNextStrongChargeCount() != 3) {
			effect_->SetAnimationActive(false);
		}
		std::unique_ptr<IPlayerBehavior> nextBehavior;
		nextBehavior.reset(new PlayerAvoidanceBehavior(host_));
		host_->TransitionBehavior(std::move(nextBehavior));
		return;
	}

	/// phaseが thirdの時に効果音をリピート再生する

	if(currentPhase_ == THIRD) {
		repeatTime_ -= Time::DeltaTime();
		if(repeatTime_ <= 0.0f) {
			repeatTime_ = repeatMaxTime_;

			host_->PlayAudio("strongAttackCharge3.wav", 0.1f);
		}
	}

}





