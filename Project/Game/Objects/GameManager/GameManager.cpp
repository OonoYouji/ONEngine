#include "GameManager.h"

/// engine
#include <imgui.h>

/// objects
#include "Objects/CameraState/CameraStateBossEntryToGame.h"


GameManager::GameManager() {
	CreateTag(this);
}

GameManager::~GameManager() {}


void GameManager::Initialize() {

}

void GameManager::Update() {

	/// ゲームの開始処理
	if(isGameStarted_) {
		if(!gameStartInitialized_) {
			gameStartInitialized_ = true;

			/// すべてのインゲームオブジェクトを起動する
			for(auto& object : inGameObjectArray_) {
				object->isActive = true;
			}

		}

	} else {

		if(pCameraState_->IsFinishedMoving()) {
			isGameStarted_ = true;
		}

	}

}

void GameManager::LastUpdate() {
}

void GameManager::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("game management flags");
		ImGui::Checkbox("is game started",        &isGameStarted_);
		ImGui::Checkbox("game start initialized", &gameStartInitialized_);

		ImGui::TreePop();
	}
}


void GameManager::SetInGameObjectArray(const std::vector<BaseGameObject*>& _inGameObjectArray) {
	inGameObjectArray_ = _inGameObjectArray;

	for(auto& object : inGameObjectArray_) {
		object->isActive = false;
	}
}
