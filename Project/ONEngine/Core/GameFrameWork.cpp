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

#ifdef _DEBUG
	pSceneManager_->Initialize(SCENE_ID::TITLE); /// 初期化時のシーンをここで決定
#else
	pSceneManager_->Initialize(SCENE_ID::TITLE); /// 初期化時のシーンをここで決定
#endif // _DEBUG



	
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
