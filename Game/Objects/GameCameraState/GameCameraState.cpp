#include "GameCameraState.h"

#include <CameraManager.h>
#include <SceneManager.h>
#include <ImGuiManager.h>
#include <Input.h>


void GameCameraState::Initialize() {
	pSceneManager_ = SceneManager::GetInstance();
	currentSceneId_ = pSceneManager_->GetCurrentScene();
	prevSceneId_ = currentSceneId_;

	/// シーンごとのカメラの位置を回転
	positions_[TITLE] = { 0,10,-50.0f };
	rotates_[TITLE] = { 0.1f, 0, 0 };

	positions_[GAME] = { 1.8f, 0.87f, -12.7f };
	rotates_[GAME] = { 0.066f, -0.258f, 0.0f };

	positions_[RESULT] = { 1.8f, 0.87f, -12.7f };
	rotates_[RESULT] = { 0.066f, 0.35f, 0.0f };

	for(uint8_t r = 0; r < 3; ++r) {
		for(uint8_t c = 0; c < 3; ++c) {
			moveTimes_[r][c] = 1.0f;
		}
	}

	BaseCamera* camera = CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ positions_[prevSceneId_], rotates_[prevSceneId_] },
		{ positions_[currentSceneId_], rotates_[currentSceneId_] },
		0.0f
	);
	camera->Move();
	camera->UpdateMatrix();

}

void GameCameraState::Update() {
	//prevSceneId_ = currentSceneId_;
	//currentSceneId_ = pSceneManager_->GetCurrentScene();

#ifdef _DEBUG
	if(Input::TriggerKey(KeyCode::F1)) {
		currentSceneId_ = TITLE;
	} else if(Input::TriggerKey(KeyCode::F2)) {
		currentSceneId_ = GAME;
	} else if(Input::TriggerKey(KeyCode::F3)) {
		currentSceneId_ = RESULT;
	}
#endif // _DEBUG

	if(prevSceneId_ != currentSceneId_) {
		BaseCamera* camera = CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->SetMove(
			{ positions_[prevSceneId_], rotates_[prevSceneId_] },
			{ positions_[currentSceneId_], rotates_[currentSceneId_] },
			moveTimes_[prevSceneId_][currentSceneId_]
		);

		prevSceneId_ = currentSceneId_;
		pSceneManager_->SetNextScene(SCENE_ID(currentSceneId_));

	}

}

void GameCameraState::Debug() {
	if(ImGui::TreeNodeEx("scene id", ImGuiTreeNodeFlags_DefaultOpen)) {

		const char* gameSceneLabels[] = { "Title", "Game", "Result" };
		// 現在選択されているシーンをインデックスとして扱います
		int currentIndex = static_cast<int>(currentSceneId_);

		// ImGui::Comboを使ってenumを表示します
		if(ImGui::Combo("Select Scene", &currentIndex, gameSceneLabels, static_cast<int>(SCENE_ID_COUNT))) {
			// 選択が変わったらenumの値を更新します
			currentSceneId_ = static_cast<SCENE_ID>(currentIndex);
		}

		ImGui::TreePop();
	}
}
