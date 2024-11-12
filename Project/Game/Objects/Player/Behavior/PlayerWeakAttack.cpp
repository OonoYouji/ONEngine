#include "PlayerWeakAttack.h"

#include "Input/Input.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerRootBehavior.h"

PlayerWeakAttack::PlayerWeakAttack(Player* player,int32_t comboNum):
	IPlayerBehavior(player),
	workInBehavior_(player->GetWorkWeakAttackBehavior(comboNum)),
	comboNum_(comboNum){
	currentUpdate_ = [this](){StartupUpdate(); };
	if(comboNum > player->GetWeakAttackComboMax()){
		host_->TransitionBehavior(std::make_unique<PlayerRootBehavior>(host_));
		return;
	}

	nextBehavior_ = std::make_unique<PlayerRootBehavior>(host_);
}

void PlayerWeakAttack::Update(){
	currentUpdate_();

	if(Input::ReleaseKey(KeyCode::LShift)){
		host_->TransitionBehavior(std::make_unique<PlayerAvoidanceBehavior>(host_));
		return;
	} else if(Input::ReleaseKey(KeyCode::J)){
		host_->TransitionBehavior(std::make_unique<PlayerWeakAttack>(host_,0));
		return;
	}
}

void PlayerWeakAttack::StartupUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.startupTime_){
		currentTime_ = 0.0f;

		currentUpdate_ = [this](){WeakAttack(); };
		return;
	}
}

void PlayerWeakAttack::WeakAttack(){
	float t = currentTime_ / workInBehavior_.motionTimes_.activeTime_;

	host_->SetDamage(workInBehavior_.damageFactor_);

	if(currentTime_ >= workInBehavior_.motionTimes_.activeTime_){
		currentTime_ = 0.0f;

		// 無敵状態 を 解除
		host_->SetIsInvisible(false);

		currentUpdate_ = [this](){EndLagUpdate(); };
		EndLagUpdate();
		return;
	}
}

void PlayerWeakAttack::EndLagUpdate(){
	if(currentTime_ >= workInBehavior_.motionTimes_.endLagTime_){
		host_->TransitionBehavior(std::move(nextBehavior_));
		return;
	}
}