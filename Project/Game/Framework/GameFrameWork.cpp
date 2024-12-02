#include "GameFrameWork.h"

/// std
#include <format>

/// engine
#include "FrameManager/ExecutionTimer.h"
#include "SceneManager/SceneManager.h"

/// game
#include "Scenes/Factory/SceneFactory.h"
#include "PostProcess/PipelineRegistry/GamePostEffectPipelineRegistry.h"


void GameFrameWork::Initialize() {
	ExecutionTimer timer;


	GamePostEffectPipelineRegistry* postEffectPipelineRegistry = new GamePostEffectPipelineRegistry();
	SceneFactory*                   sceneFacory                = new SceneFactory("GameScene");

	pSceneManager_ = SceneManager::GetInstance();
	pSceneManager_->Initialize(sceneFacory, postEffectPipelineRegistry);

	
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

