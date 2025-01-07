#include "GameFrameWork.h"

/// std
#include <format>

/// engine
#include "FrameManager/ExecutionTimer.h"
#include "SceneManager/SceneManager.h"

/// game
#include "Scenes/Factory/SceneFactory.h"


void GameFrameWork::Initialize() {
	ExecutionTimer timer;

	SceneFactory* sceneFacory = new SceneFactory("TitleScene");
	/// シーンの初期化等...
	pSceneManager_ = SceneManager::GetInstance();
	pSceneManager_->Initialize(sceneFacory); /// 初期化時のシーンをここで決定

	
	ONE::Logger::ConsolePrint(std::format(
		"GameFrameWork::Initialize() Time: {}s",
		timer.EndTimer(TIME_UNIT_SECONDS)
	));
}

void GameFrameWork::Update() {
	pSceneManager_->Update();
}

void GameFrameWork::Draw() {
	pSceneManager_->Draw();
}

void GameFrameWork::Finalize() {
	pSceneManager_ = nullptr;
}

