#include "GameOperationUI.h"

#include <ModelManager.h>
#include <Input.h>

void GameOperationUI::Initialize() {
	drawLayerId = 1;
	paper_ = ModelManager::Load("paper");
	paperMaterial_.CreateMaterial("sousa");

	binder_ = ModelManager::Load("binder");

	SetPosition({ -5.3f, 0.6f, 1.0f });
	SetRotate({ -1.15f, -0.4f, 0.0f });
	SetScale(Vec3::kOne * 0.7f);
	UpdateMatrix();

}

void GameOperationUI::Update() {
}

void GameOperationUI::Draw() {
	paper_->Draw(&transform_, &paperMaterial_);
	binder_->Draw(&transform_);
}
