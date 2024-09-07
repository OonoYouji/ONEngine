#include "GameMonitor.h"

#include <ModelManager.h>


void GameMonitor::Initialize() {
	drawLayerId = 1;
	tv_ = ModelManager::Load("TV");
	screen_= ModelManager::Load("GameScreen");
	screenMaterial_.CreateMaterial("monitor");
}

void GameMonitor::Update() {}

void GameMonitor::Draw() {
	tv_->Draw(&transform_);
	screen_->Draw(&transform_);
	screen_->Draw(&transform_, &screenMaterial_);
}
