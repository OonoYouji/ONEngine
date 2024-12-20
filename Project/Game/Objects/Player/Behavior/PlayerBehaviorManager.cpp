#include "PlayerBehaviorManager.h"

/// std
#include <format>
#include <numbers>

/// externals
#include <imgui.h>

/// game
#include "CustomMath/Json/CreateJsonFile.h"
#include "CustomMath/ImGui/Combo.h"
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



	selectedEditMotionIndex_ = 0;
	selectMotionKey_ = "default";

	motions_["default"].reset(new BaseMotion(pPlayer_));
	motions_["aaa"].reset(new BaseMotion(pPlayer_));
	motions_["bbb"].reset(new BaseMotion(pPlayer_));
	motions_["ccc"].reset(new BaseMotion(pPlayer_));

}


void PlayerBehaviorManager::Update() {

	motions_[selectMotionKey_]->Update();


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

void PlayerBehaviorManager::AddMotion(BaseMotion* _motion) {
	if(ImGui::Button("add")) {

	}
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


