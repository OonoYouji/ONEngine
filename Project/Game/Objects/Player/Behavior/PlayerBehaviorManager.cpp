#include "PlayerBehaviorManager.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// game
#include "../Player.h"

/// behavior
#include "PlayerRootBehavior.h"
#include "PlayerDushBehavior.h"
#include "PlayerJumpBehavior.h"



PlayerBehaviorManager::PlayerBehaviorManager(Player* _player)
	: pPlayer_(_player) {

}

PlayerBehaviorManager::~PlayerBehaviorManager() {}

void PlayerBehaviorManager::Initialize() {
	AddBehavior("dush", new PlayerDushBehavior(pPlayer_));
	AddBehavior("jump", new PlayerJumpBehavior(pPlayer_));
	AddBehavior("root", new PlayerRootBehavior(pPlayer_));

	currentBehavior_ = behaviorMap_.at("root").get();

	VariableManager* vm = VariableManager::GetInstance();
	for(auto& behavior : behaviorMap_) {
		vm->LoadSpecificGroupsToJson(
			BasePlayerBehavior::sDirectoryPath_,
			behavior.second->kName_
		);
	}
}

void PlayerBehaviorManager::Update() {
	if(currentBehavior_) {
		currentBehavior_->Update();


		/// 次のbehaviorに飛ぶ
		if(currentBehavior_->CanExit()) {
			/// 次のbehaviorを確認、あれば遷移
			std::string next = currentBehavior_->GetNextBehavior();
			if(behaviorMap_.count(next)) {
				/// 終了時、スタート時に呼ぶ関数をコール
				currentBehavior_->Exit();
				currentBehavior_ = behaviorMap_.at(next).get();
				currentBehavior_->Start();
			}
		}
	}
}



void PlayerBehaviorManager::AddBehavior(const std::string& _name, BasePlayerBehavior* _addBehavior) {
	std::unique_ptr<BasePlayerBehavior> add(_addBehavior);
	behaviorMap_[_name] = std::move(add);
}

void PlayerBehaviorManager::Debugging() {
	if(ImGui::TreeNode("BehaviorManager")) {

		ImGui::SeparatorText("debug");
		ImGui::Text(
			std::format("current behavior: \"{}\"", currentBehavior_->kName_).c_str()
		);

		ImGui::SeparatorText("behaviors");

		VariableManager* vm = VariableManager::GetInstance();
		for(auto& behavior : behaviorMap_) {
			if(ImGui::TreeNodeEx(
				std::format("\"{}\"", behavior.second->kName_).c_str(),
				ImGuiTreeNodeFlags_Framed
				)) {


				if(ImGui::Button(std::format(
					"save file##{:p}",
					reinterpret_cast<void*>(behavior.second.get())).c_str())) {

					vm->SaveSpecificGroupsToJson(behavior.second->sDirectoryPath_, behavior.second->kName_);
				}
				vm->DebuggingSpecificGroup(behavior.second->kName_);

				ImGui::TreePop();
			}
			
			ImGui::Spacing();
			ImGui::Separator();
		}

		ImGui::TreePop();
	}
}



void PlayerBehaviorManager::Editor() {


}
