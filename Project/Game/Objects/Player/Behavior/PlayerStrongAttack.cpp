#define NOMINMAX
#include "PlayerStrongAttack.h"

/// std
#include <algorithm>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Input/Input.h"

#include "../Player.h"
#include "PlayerRootBehavior.h"

PlayerStrongAttack::PlayerStrongAttack(Player* _player) : IPlayerBehavior(_player) {

	currentPhase_ = NONE;
	currentTime_  = 0.0f;

	for(size_t i = 0; i < chargePhaseDataArray_.size(); ++i) {
		chargePhaseDataArray_[i].time = (i) * 2.0f;
	}

	

	{	/// 値のio

		VariableManager* vm = VariableManager::GetInstance();
		const std::string groupName = "StrongAttackBehavior";

		vm->AddValue(groupName, "startLagTime", startLagTime_);
		vm->AddValue(groupName, "endLagTime",   endLagTime_);

		//vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

		startLagTime_ = vm->GetValue<float>(groupName, "startLagTime");
		endLagTime_   = vm->GetValue<float>(groupName, "endLagTime");

	}


}


void PlayerStrongAttack::Update() {

	bool isEnd = false;
	isEnd |= Input::ReleaseKey(KeyCode::K);
	isEnd |= Input::ReleasePadButton(PadCode::A);

	/// 入力をやめた瞬間が攻撃する瞬間
	if(isEnd) {
		host_->TransitionBehavior(std::make_unique<PlayerRootBehavior>(host_));
		return;
	}


	currentTime_ += Time::DeltaTime();

	ChargePhaseData& nextData = chargePhaseDataArray_[std::min(currentPhase_ + 1, static_cast<int>(COUNT - 1))];
	if(currentTime_ >= nextData.time) {

		/// 次のphaseに行く
		++currentPhase_;
		currentPhase_ = std::clamp(currentPhase_, 0, static_cast<int>(COUNT - 1));


		/// アニメーションを設定する
		host_->SetAnimationModel(
			"KariPlayer_StrongAttack" + std::to_string(currentPhase_)
		);


		if(currentPhase_ == THIRD) {
			isChargeMax_ = true;
		}
	}

}




