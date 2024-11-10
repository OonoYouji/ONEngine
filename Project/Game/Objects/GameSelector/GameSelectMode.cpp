#include "GameSelectMode.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


GameSelectMode::GameSelectMode() {
	CreateTag(this);
}

GameSelectMode::~GameSelectMode() {}

void GameSelectMode::Initialize() {

	pTransform_->scale.y = 0.4f;
	pTransform_->scale.x = pTransform_->scale.y * 2.0f;

}

void GameSelectMode::Update() {


	/// xとyの比率が 2:1 なので
	pTransform_->scale.x = pTransform_->scale.y * 2.0f;
}


template<>
void GameSelectMode::ThisInitialize<GAME_SELECT_MODE_START>() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("start.png");

}

template<>
void GameSelectMode::ThisInitialize<GAME_SELECT_MODE_EXIT>() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("exit.png");

	pTransform_->position.y = -0.5f;

}

