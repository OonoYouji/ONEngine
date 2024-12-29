#include "PlayerNormalAttack.h"

/// std
#include <algorithm>

/// external
#include <imgui.h>

/// engine
#include "FrameManager/Time.h"
#include "VariableManager/VariableManager.h"

/// user
#include "../Manager/PlayerStateManager.h"


PlayerNormalAttack::PlayerNormalAttack(Player* _player, PlayerStateManager* _stateManager) : IPlayerState(_player, _stateManager) {}
PlayerNormalAttack::~PlayerNormalAttack() {}

void PlayerNormalAttack::Start() {
	time_       = 0.0f;
	attackData_ = sourceAttackData_;

	isEnded_ = false;
}

void PlayerNormalAttack::Update() {

	time_ += Time::DeltaTime();
	float lerpT = std::clamp(time_ / maxTime_, 0.0f, 1.0f);


	
	/// 攻撃の終了
	if(lerpT == 1.0f) {
		isEnded_ = true;
		return;
	}

}

void PlayerNormalAttack::Exit() {

}

bool PlayerNormalAttack::IsEnd() {
	return isEnded_;
}

int PlayerNormalAttack::NextStateIndex() {
	if(isEnded_) {
		return PlayerStateOrder_Root;
	}

	return 0;
}

void PlayerNormalAttack::Debug() {
	if(ImGui::TreeNode("NormalAttack")) {


		/// save / load のボタンを追加
		if(ImGui::Button("save")) {
			Save();
		}

		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();

		if(ImGui::Button("load")) {
			Load();
		}



		ImGui::Spacing();

		ImGui::DragFloat("max time", &maxTime_, 0.05f);

		/// sourceAttackDataのdebug
		if(ImGui::TreeNode("source attack data")) {
			ImGui::DragInt("hit count",      &sourceAttackData_.hitCount, 1, 0, 100);
			ImGui::DragFloat("hit interval", &sourceAttackData_.hitInterval, 0.005f);
			ImGui::DragFloat("damage",       &sourceAttackData_.damage,      0.05f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void PlayerNormalAttack::Save() {

}

void PlayerNormalAttack::Load() {

}

