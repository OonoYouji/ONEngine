#include "Heart.h"

#include <ModelManager.h>


void Heart::Initialize() {

	drawLayerId = 1;

	bottom_ = new HeartBottom;
	bottom_->Initialize();
	bottom_->drawLayerId = drawLayerId;

	above_ = new HeartAbove;
	above_->Initialize();
	above_->drawLayerId = drawLayerId;

	bottom_->SetParent(this);
	above_->SetParent(this);
}

void Heart::Update() {}

void Heart::Draw() {
}



/// ===================================================
/// 心臓の下部
/// ===================================================

void HeartBottom::Initialize() {
	model_ = ModelManager::Load("HeartBottom");
}

void HeartBottom::Update() {}

void HeartBottom::Draw() {
	model_->Draw(&transform_);
}



/// ===================================================
/// 心臓の下部
/// ===================================================

void HeartAbove::Initialize() {
	model_ = ModelManager::Load("HeartAbove");
}

void HeartAbove::Update() {}

void HeartAbove::Draw() {
	model_->Draw(&transform_);
}
