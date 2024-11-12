#include "GameFrameWork.h"

/// std
#include <format>

/// engine
#include "ONEngine.h"
#include "FrameManager/ExecutionTimer.h"

/// game
#include "Scenes/Manager/SceneManager.h"


void GameFrameWork::Initialize() {
	ExecutionTimer timer;

	/// シーンの初期化等...
	pSceneManager_ = SceneManager::GetInstance();
	pSceneManager_->Initialize(SCENE_ID::RESULT); /// 初期化時のシーンをここで決定

	
	ONE::Logger::ConsolePrint(std::format(
		"GameFrameWork::Initialize() Time: {}s",
		timer.EndTimer(TIME_UNIT_SECONDS)
	));
}

void GameFrameWork::Update() {
	ONEngine::Update();
	pSceneManager_->Update();
}

void GameFrameWork::Draw() {
	ONEngine::PreDraw();
	pSceneManager_->Draw();
	ONEngine::PostDraw();
}

void GameFrameWork::Finalize() {
	pSceneManager_ = nullptr;
}



void GameFrameWork::Run() {
	Initialize();

	while(ONEngine::GetIsRunning()) {
		Update();
		Draw();
	}

	Finalize();
}
