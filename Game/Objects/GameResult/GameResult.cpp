#include "GameResult.h"

#include <ModelManager.h>

void GameResult::Initialize() {
	paper_ = ModelManager::Load("paper");
	binder_ = ModelManager::Load("binder");
}

void GameResult::Update() {

}

void GameResult::Draw() {
	paper_->Draw(&transform_);
	binder_->Draw(&transform_);
}
