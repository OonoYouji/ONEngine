#include "Heart.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <WorldTime.h>

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

void Heart::Draw() {}



/// ===================================================
/// 心臓の下部
/// ===================================================

void HeartBottom::Initialize() {
	model_ = ModelManager::Load("HeartBottom");
}

void HeartBottom::Update() {
	animationTime_ += WorldTime::DeltaTime();

	float heartBeat = amplitude_ * (std::sin(speed_ * animationTime_) * 0.5f + 0.5f);
	transform_.scale = Vec3::kOne + (Vec3::kOne * heartBeat);
}

void HeartBottom::Draw() {
	model_->Draw(&transform_);
}

void HeartBottom::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &speed_, 0.001f);
		ImGui::DragFloat("amplitude", &amplitude_, 0.001f);

		ImGui::TreePop();
	}
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
