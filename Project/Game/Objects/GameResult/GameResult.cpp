#include "GameResult.h"

#include "BackTitleUI/BackTitleUI.h"
#include "RetryUI/RetryUI.h"

GameResult::GameResult() {
	CreateTag(this);
}

GameResult::~GameResult() {}

void GameResult::Initialize() {
	retryUI_     = new RetryUI();
	backTitleUI_ = new BackTitleUI();

	retryUI_->Initialize();
	backTitleUI_->Initialize();

}

void GameResult::Update() {

}
