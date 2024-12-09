#include "PlayerBehaviorManager.h"

/// std
#include <format>
#include <numbers>

/// externals
#include <imgui.h>

/// game
#include "CustomMath/Json/CreateJsonFile.h"
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

	tmp.reset(new BaseMotion(pPlayer_));
}


void PlayerBehaviorManager::Update() {

	tmp->Update();


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

		//ImGui::SeparatorText("debug");
		//ImGui::Text(
		//	std::format("current behavior: \"{}\"", currentBehavior_->kName_).c_str()
		//);

		//ImGui::SeparatorText("behaviors");

		//VariableManager* vm = VariableManager::GetInstance();
		//for(auto& behavior : behaviorMap_) {
		//	if(ImGui::TreeNodeEx(
		//		std::format("\"{}\"", behavior.second->kName_).c_str(),
		//		ImGuiTreeNodeFlags_Framed
		//		)) {


		//		if(ImGui::Button(std::format(
		//			"save file##{:p}",
		//			reinterpret_cast<void*>(behavior.second.get())).c_str())) {

		//			vm->SaveSpecificGroupsToJson(behavior.second->sDirectoryPath_, behavior.second->kName_);
		//		}
		//		vm->DebuggingSpecificGroup(behavior.second->kName_);

		//		ImGui::TreePop();
		//	}
		//	
		//	ImGui::Spacing();
		//	ImGui::Separator();
		//}


		//AddMotion(tmp.get());
		MotionEdit(tmp.get());

		ImGui::TreePop();
	}
}

void PlayerBehaviorManager::AddMotion(BaseMotion* _motion) {
	if(ImGui::Button("add")) {

	}
}

void PlayerBehaviorManager::MotionEdit(BaseMotion* _motion) {
	if(ImGui::TreeNode("motion edit")) {

		if(ImGui::Button("save json")) {
			SaveMotionToJson("test", _motion);
		}

		/// 再生する
		if(ImGui::Button("play")) {
			_motion->Start();
		}

		ImGui::Separator();


		ImGui::DragFloat("current time", &_motion->currentTime_);
		ImGui::DragFloat3("current keyfrarme position", &_motion->currentKeyframe_.position.x, 0.0f);
		ImGui::DragFloat3("current keyfrarme rotate",   &_motion->currentKeyframe_.rotate.x,   0.0f);
		ImGui::DragFloat3("current keyfrarme scale",    &_motion->currentKeyframe_.scale.x,    0.0f);

		ImGui::Spacing();

		ImGui::DragFloat("max time", &_motion->maxTime_, 0.025f);

		ImGui::Spacing();

		if(ImGui::Button("add keyframe")) {
			_motion->keyframes_.push_back(_motion->keyframes_.back());
		}

		if(ImGui::TreeNode("keyframes")) {

			size_t index = 0;
			for(auto& keyframe : _motion->keyframes_) {
				ImGui::Text("index(%d)", index);
				
				ImGui::DragFloat(std::format("time##{:p}", reinterpret_cast<void*>(&keyframe)).c_str(), &keyframe.time, 0.1f);
				
				ImGui::Spacing();

				ImGui::DragFloat3(std::format("position##{:p}", reinterpret_cast<void*>(&keyframe)).c_str(), &keyframe.position.x, 0.1f);
				ImGui::DragFloat3(std::format("rotate##{:p}", reinterpret_cast<void*>(&keyframe)).c_str(), &keyframe.rotate.x, std::numbers::pi_v<float> * 0.1f);
				ImGui::DragFloat3(std::format("scale##{:p}", reinterpret_cast<void*>(&keyframe)).c_str(), &keyframe.scale.x, 0.1f);

				index++;
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

}

void PlayerBehaviorManager::SelectEditMotion() {
	//ImGui::Combo();
}



void PlayerBehaviorManager::SaveMotionToJson(const std::string& _fileName, BaseMotion* _motion) {

	json root = json::object();
	
	for(size_t i = 0; i < _motion->keyframes_.size(); ++i) {
		MotionKeyframe& keyframe = _motion->keyframes_[i];
		json& item = root[std::to_string(i)];

		item["time"] = keyframe.time;
		item["position"] = json::array({ keyframe.position.x, keyframe.position.y, keyframe.position.z });
		item["rotate"] = json::array({ keyframe.rotate.x, keyframe.rotate.y, keyframe.rotate.z });
		item["scale"] = json::array({ keyframe.scale.x, keyframe.scale.y, keyframe.scale.z });
	}


	CreateJsonFile("./Resources/Parameters/Player/", "TestMotion", root);

}


