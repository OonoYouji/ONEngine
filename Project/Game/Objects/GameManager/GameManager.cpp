#include "GameManager.h"

/// engine
#include <imgui.h>


GameManager::GameManager() {
	CreateTag(this);
}

GameManager::~GameManager() {}


void GameManager::Initialize() {

}

void GameManager::Update() {

	if(!preIsGameStarted_ && isGameStarted_) {

	}

}

void GameManager::LastUpdate() {
	/// ゲームが開始した瞬間を判断するため prevを取る
	preIsGameStarted_ = isGameStarted_;
}

void GameManager::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("game management flags");
		ImGui::Checkbox("is game started", &isGameStarted_);

		ImGui::TreePop();
	}
}


void GameManager::SetInGameObjectArray(const std::vector<BaseGameObject*>& _inGameObjectArray) {
	inGameObjectArray_ = _inGameObjectArray;
}
