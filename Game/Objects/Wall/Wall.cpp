#include "Wall.h"

#include <numbers>

#include <ModelManager.h>

void Wall::Initialize() {
	model_ = ModelManager::Load("wall");
	drawLayerId = 1;

	SetPosition({ 0, 0, 5 });
	SetRotate({ 0, 1.0f * std::numbers::pi_v<float>, 0 });
	SetScale({ 3,3,3 });

}

void Wall::Update() {

}

void Wall::Draw() {
	model_->Draw(&transform_);
}
