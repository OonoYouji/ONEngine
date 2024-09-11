#include "Hand.h"

#include <algorithm>

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <WorldTime.h>

#include "LineDrawer2D/SinWaveDrawer.h"

void Hand::Initialize() {
	drawLayerId = 1;

	finger_ = new HandFinger;
	finger_->Initialize();
	finger_->SetParent(this);

	palm_ = new HandPalm;
	palm_->Initialize();
	palm_->SetParent(this);

	thumb_ = new HandThumb;
	thumb_->Initialize();
	thumb_->SetParent(this);

}

void Hand::Update() {

}

void Hand::Draw() {}


/// ===================================================
/// 人差し指から小指までのオブジェクト
/// ===================================================

void HandFinger::Initialize() {
	drawLayerId = 1;
	model_ = ModelManager::Load("HandFinger");
	speed_ = -8.0f;
	amplitude_ = 0.5f;

	pSinWaveDrawer_ = dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer"));
	//assert(pSinWaveDrawer_ != nullptr);
}

void HandFinger::Update() {
	if(!pSinWaveDrawer_) { return; }

	animationTime_ += WorldTime::DeltaTime() * (pSinWaveDrawer_->GetAmplitude() / 100.0f);

	float beat = amplitude_ * (std::sin(speed_ * animationTime_) * 0.5f + 0.5f);
	transform_.rotate.x = -beat;
	transform_.rotate.y = beat;
}

void HandFinger::Draw() {
	model_->Draw(&transform_);
}

void HandFinger::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &speed_, 0.01f);
		ImGui::DragFloat("amplitude", &amplitude_, 0.01f);
		ImGui::TreePop();
	}
}


/// ===================================================
/// 親指のオブジェクト
/// ===================================================

void HandThumb::Initialize() {
	drawLayerId = 1;
	model_ = ModelManager::Load("HandThumb");
	speed_ = -8.0f;
	amplitude_ = -0.5f;

	pSinWaveDrawer_ = dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer"));
	//assert(pSinWaveDrawer_ != nullptr);
}

void HandThumb::Update() {
	if(!pSinWaveDrawer_) { return; }

	animationTime_ += WorldTime::DeltaTime() * (pSinWaveDrawer_->GetAmplitude() / 100.0f);

	float beat = amplitude_ * (std::sin(speed_ * animationTime_) * 0.5f + 0.5f);
	transform_.rotate.y = beat;
}

void HandThumb::Draw() {
	model_->Draw(&transform_);
}

void HandThumb::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &speed_, 0.01f);
		ImGui::DragFloat("amplitude", &amplitude_, 0.01f);
		ImGui::TreePop();
	}
}


/// ===================================================
/// 手のひらのオブジェクト
/// ===================================================

void HandPalm::Initialize() {
	drawLayerId = 1;
	model_ = ModelManager::Load("HandPalm");
}

void HandPalm::Update() {}

void HandPalm::Draw() {
	model_->Draw(&transform_);
}
