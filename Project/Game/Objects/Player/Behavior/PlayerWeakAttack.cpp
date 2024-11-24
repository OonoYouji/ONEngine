#include "PlayerWeakAttack.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Input/Input.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

/// player
#include "PlayerAvoidanceBehavior.h"
#include "PlayerRootBehavior.h"
#include "../Collision/PlayerAttackCollider/PlayerAttackCollider.h"


PlayerWeakAttack::PlayerWeakAttack(Player* player, int32_t comboNum) :
	IPlayerBehavior(player),
	comboNum_(comboNum) {
	currentUpdate_ = [this]() {StartupUpdate(); };

	workInBehavior_ = player->GetWorkWeakAttackBehavior(comboNum_);

	nextBehavior_ = static_cast<int>(NextBehavior::root);

	std::string animationModelFilePath = "Player_WeakAttack";
	animationModelFilePath += std::to_string(comboNum_ + 1);


	host_->SetDamage(workInBehavior_.damageFactor_);
	host_->SetAttackMode(PlayerAttackCollider::MODE_WEAK_ATTACK);

	host_->SetIsActiveWeapon(true);
	host_->SetAnimationModel(
		animationModelFilePath + "_1_P",
		animationModelFilePath + "_1_W"
	);
	host_->SetAnimationTotalTime(workInBehavior_.motionTimes_.startupTime_);
	host_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);
}

void PlayerWeakAttack::Update() {
	currentTime_ += Time::DeltaTime();
	currentUpdate_();

	bool isNextCombo = false;
	bool isDush      = false;

	/// 回避のための入力
	isDush      |= Input::TriggerKey(KeyCode::LShift);
	isDush      |= Input::TriggerPadRT();

	/// 次のコンボのための入力
	isNextCombo |= Input::TriggerKey(KeyCode::J);
	isNextCombo |= Input::TriggerPadButton(PadCode::B);


	/// 次のbehaviorに行くための処理
	if(isDush) {
		nextBehavior_ = static_cast<int>(NextBehavior::avoidance);
		return;
	} else if(isNextCombo) {
		// comboNum_が範囲外（0未満または最大コンボ数以上）の場合にreturn
		if(comboNum_ < 0 || comboNum_ >= host_->GetWeakAttackComboMax()) {
			return;
		}

		nextBehavior_ = static_cast<int>(NextBehavior::combo);
		return;
	}
}

void PlayerWeakAttack::StartupUpdate() {
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_) {
		currentTime_ = 0.0f;


		std::string animationModelFilePath = "Player_WeakAttack";
		animationModelFilePath += std::to_string(comboNum_ + 1);

		host_->SetAnimationModel(
			animationModelFilePath + "_2_P",
			animationModelFilePath + "_2_W"
		);
		host_->SetAnimationTotalTime(workInBehavior_.motionTimes_.activeTime_);
		host_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);


		currentUpdate_ = [this]() {WeakAttack(); };
		return;
	}
}

void PlayerWeakAttack::WeakAttack() {
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	host_->SetDamage(workInBehavior_.damageFactor_);

	host_->GetAttackCollider()->isActive = true;

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_) {
		currentTime_ = 0.0f;

		// 無敵状態 を 解除
		host_->SetIsInvisible(false);
		host_->GetAttackCollider()->isActive = false;


		std::string animationModelFilePath = "Player_WeakAttack";
		animationModelFilePath += std::to_string(comboNum_ + 1);

		host_->SetAnimationModel(
			animationModelFilePath + "_3_P",
			animationModelFilePath + "_3_W"
		);
		host_->SetAnimationTotalTime(workInBehavior_.motionTimes_.endLagTime_);
		host_->SetAnimationFlags(ANIMATION_FLAG_NOLOOP);


		currentUpdate_ = [this]() {EndLagUpdate(); };
		EndLagUpdate();
		return;
	}
}

void PlayerWeakAttack::EndLagUpdate() {
	if(nextBehavior_ == static_cast<int>(NextBehavior::combo)) {
		host_->SetIsActiveWeapon(false);

		std::unique_ptr<IPlayerBehavior> nextBehavior;

		switch(nextBehavior_) {
		case static_cast<int>(NextBehavior::avoidance):
			nextBehavior = std::make_unique<PlayerAvoidanceBehavior>(host_);
			break;
		case static_cast<int>(NextBehavior::combo):
			nextBehavior = std::make_unique<PlayerWeakAttack>(host_, comboNum_ + 1);
			break;
		}

		host_->TransitionBehavior(std::move(nextBehavior));
		return;
	}





	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_) {
		host_->SetIsActiveWeapon(false);

		host_->SetAnimationModel(
			"Player_Wait"
		);
		host_->SetAnimationTotalTime(host_->GetAnimationDuration());
		host_->SetAnimationFlags(0);


		std::unique_ptr<IPlayerBehavior> nextBehavior;

		switch(nextBehavior_) {
		case static_cast<int>(NextBehavior::root):
			nextBehavior = std::make_unique<PlayerRootBehavior>(host_);
			break;
		case static_cast<int>(NextBehavior::avoidance):
			nextBehavior = std::make_unique<PlayerAvoidanceBehavior>(host_);
			break;
		case static_cast<int>(NextBehavior::combo):
			nextBehavior = std::make_unique<PlayerWeakAttack>(host_, comboNum_ + 1);
			break;
		}

		host_->TransitionBehavior(std::move(nextBehavior));


		return;
	}
}