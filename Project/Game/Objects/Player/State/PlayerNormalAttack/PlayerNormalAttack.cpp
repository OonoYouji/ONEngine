#include "PlayerNormalAttack.h"

/// std
#include <algorithm>
#include <fstream>

/// external
#include <imgui.h>
#include <nlohmann/json.hpp>

/// engine
#include "FrameManager/Time.h"
#include "VariableManager/VariableManager.h"

/// user
#include "../Manager/PlayerStateManager.h"

using namespace nlohmann;


PlayerNormalAttack::PlayerNormalAttack(Player* _player, PlayerStateManager* _stateManager) 
	: IPlayerState(_player, _stateManager) {
	Load();
}
PlayerNormalAttack::~PlayerNormalAttack() {}

void PlayerNormalAttack::Start() {
	time_ = 0.0f;
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

		ImGui::Spacing();

		if(ImGui::Button("add bounce position")) {
			bouncePositions_.push_back(Vec3());
		}


		if(ImGui::BeginChild("positions", ImVec2(0.0f, 320.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
			for(int i = 0; i < bouncePositions_.size(); ++i) {
				ImGui::PushID(i);
				ImGui::DragFloat3("pos", &bouncePositions_[i].x, 0.05f);
				ImGui::PopID();
			}
			ImGui::EndChild();
		}




		ImGui::TreePop();
	}
}

void PlayerNormalAttack::Save() {

	const std::string filePath = "./Resources/Parameters/Objects/PlayerNormalAttack.json";

	/// jsonファイルに保存
	json root;
	root["attackData"] = json::array();

	/// EmitterDataをjsonに変換して追加
	for(const auto& data : bouncePositions_) {
		json j;
		j["position"] = { data.x, data.y, data.z };
		root["attackData"].push_back(j);
	}

	/// jsonファイルに書き込み
	std::ofstream ofs(filePath);
	ofs << std::setw(4) << root << std::endl;

	/// 成功したらポップアップウィンドウを出す
	if(ofs) {
		MessageBoxA(nullptr, "save success", "PlayerNormalAttack", MB_OK | MB_ICONINFORMATION);
	}


}

void PlayerNormalAttack::Load() {

	const std::string filePath = "./Resources/Parameters/Objects/PlayerNormalAttack.json";

	/// jsonファイルを読み込み
	std::ifstream ifs(filePath);
	if(!ifs.is_open()) {
		MessageBoxA(nullptr, "file not found", "PlayerNormalAttack", MB_OK | MB_ICONERROR);
		return;
	}

	json root;
	ifs >> root;

	/// positionsの読み込み
	bouncePositions_.clear();
	for(const auto& data : root["attackData"]) {
		Vec3 pos;
		pos.x = data["position"][0];
		pos.y = data["position"][1];
		pos.z = data["position"][2];
		bouncePositions_.push_back(pos);
	}

}

