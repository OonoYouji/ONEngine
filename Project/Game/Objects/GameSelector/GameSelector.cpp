#include "GameSelector.h"

/// std
#include <algorithm>

/// lib
#include "Input/Input.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// objects
#include "GameSelectMode.h"


GameSelector::GameSelector() {
	CreateTag(this);
}

GameSelector::~GameSelector() {}

void GameSelector::Initialize() {

	/// ゲームのモード選択
	gameSelectMode_ = GAME_SELECT_MODE_START;



	/// start, exitのスプライト初期化
	for(size_t i = 0; i < (GAME_SELECT_MODE_COUNT); ++i) {
		gameSelectModeArray_[i] = new GameSelectMode();
		gameSelectModeArray_[i]->Initialize();

		/// enum ごとで初期化を変える
		if(i == GAME_SELECT_MODE_START) {
			gameSelectModeArray_[i]->ThisInitialize<GAME_SELECT_MODE_START>();
		} else {
			gameSelectModeArray_[i]->ThisInitialize<GAME_SELECT_MODE_EXIT>();
		}
	}



	/// spriteの初期化 (right arrow.png)
	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("rightArrow.png");


	pTransform_->position.x = -0.5f;
	pTransform_->scale = { 1.0f, 0.5f, 1.0f };

}

void GameSelector::Update() {

	if(Input::TriggerKey(KeyCode::W)) { gameSelectMode_--; } 
	if(Input::TriggerKey(KeyCode::S)) { gameSelectMode_++; }

	/// 範囲外にいかないようクランプ
	gameSelectMode_ = std::clamp(gameSelectMode_, 0, static_cast<int>(GAME_SELECT_MODE_COUNT - 1));

	pTransform_->position.y = gameSelectModeArray_[gameSelectMode_]->GetPosition().y;
}

