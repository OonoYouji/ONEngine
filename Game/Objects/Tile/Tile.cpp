#include "Tile.h"

#include <ModelManager.h>


void Tile::Initialize() {
	model_ = ModelManager::Load("tile");
	drawLayerId = 1;

	SetPosition({ 0.0f, -2.5f, 0.0f });
	SetScale(Vec3::kOne * 2.0f);

}

void Tile::Update() {

}

void Tile::Draw() {
	model_->Draw(&transform_);
}
