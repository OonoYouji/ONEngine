#include "GameResult.h"

void GameResult::Initialize() {
	sprite_.reset(new Sprite);
	sprite_->Initialize("uvChecker", "uvChecker.png");
	sprite_->SetPos({640.0f, 360.0f, 0.0f});
	sprite_->SetSize({100.0f, 100.0f});
}

void GameResult::Update() {

}

void GameResult::BackSpriteDraw() {
	sprite_->Draw();
}
