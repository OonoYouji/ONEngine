#include "PlayerWeakAttack.h"

#include "FrameManager/Time.h"
#include "Input/Input.h"
#include "PlayerAvoidanceBehavior.h"
#include "PlayerRootBehavior.h"

PlayerWeakAttack::PlayerWeakAttack(Player* player,int32_t comboNum):
	IPlayerBehavior(player),
	comboNum_(comboNum){
	currentUpdate_ = [this](){StartupUpdate(); };

	workInBehavior_ = player->GetWorkWeakAttackBehavior(comboNum_);

	nextBehavior_ = std::make_unique<PlayerRootBehavior>(host_);

	std::string animationModelFilePath = "KariPlayer_WeakAttack";
	if(comboNum_) {
		animationModelFilePath += std::to_string(comboNum_);
	}

	host_->SetAnimationModel(animationModelFilePath);

}

void PlayerWeakAttack::Update(){
	currentTime_ += Time::DeltaTime();
	currentUpdate_();

	if(Input::ReleaseKey(KeyCode::LShift)){
		nextBehavior_ = std::make_unique<PlayerAvoidanceBehavior>(host_);
		return;
	} else if(Input::TriggerKey(KeyCode::J)){
		// comboNum_が範囲外（0未満または最大コンボ数以上）の場合にreturn
		if(comboNum_ < 0 || comboNum_ >= host_->GetWeakAttackComboMax()){
			return;
		}

		nextBehavior_ = std::make_unique<PlayerWeakAttack>(host_,comboNum_ + 1);
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