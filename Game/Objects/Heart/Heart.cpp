#include "Heart.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <WorldTime.h>

#include "LineDrawer2D/SinWaveDrawer.h"

void Heart::Initialize() {

	drawLayerId = 1;
	transform_.scale = Vec3::kOne * 1.2f;
	transform_.position = Vec3(0.5f, -0.3f, -1.2f);
	transform_.rotate.z = 0.3f;

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
	animationTime_ = 0.0f;
	amplitude_ = 0.6f;

	pSinWaveDrawer_ = dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer"));
	//assert(pSinWaveDrawer_ != nullptr);
}

void HeartBottom::Update() {
	if(!pSinWaveDrawer_) { return; }

	animationTime_ += WorldTime::DeltaTime() * (pSinWaveDrawer_->GetAmplitude() / 100.0f);

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
