#include "GameManager.h"

/// objects
#include "Objects/RailCamera/RailCamera.h"

GameManager::GameManager() {
	CreateTag(this);
}

GameManager::~GameManager() {}

void GameManager::Initialize() {

}

void GameManager::Update() {

	/// レールカメラが移動終了していればゲームが終了した
	if(pRailCamera_->GetIsMoveEnd()) {
		isGameEnd_ = true;
	}

}

void GameManager::SetRailCamera(RailCamera* _railCamera) {
	pRailCamera_ = _railCamera;
}

