#define NOMINMAX
#include "GameOperationUI.h"

#include <ImGuiManager.h>
#include <SceneManager.h>
#include <ModelManager.h>
#include <Input.h>
#include <WorldTime.h>
#include <AudioSource.h>

#include "GameManager/GameManager.h"

void GameOperationUI::Initialize() {
	drawLayerId = 1;
	paper_ = ModelManager::Load("paper");
	paperMaterial_.CreateMaterial("sousa");
	paperMaterial_.SetIsLighting(false);
	paperColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
	paperMaterial_.SetColor(paperColor_);

	binder_ = ModelManager::Load("binder");

	SetPosition({ -6.0f, 0.6f, 1.0f });
	SetRotate({ -1.15f, -0.4f, 0.0f });
	SetScale(Vec3::kOne * 0.7f);
	UpdateMatrix();

	/// {-3.6f, 0.15f, -4.7f}
	maxLerpTime_ = 0.2f;

	sousa_ = new AudioSource();
	sousa_->SetAudioClip("sousa.wav");

	isFrontBinder_ = GameManager::initializeIsPause;

}

void GameOperationUI::Update() {

	/// enter を押すか　game pad RB を押す
	if(Input::TriggerKey(KeyCode::Enter)
	   || Input::TriggerPadButton(PadCode::RightShoulder)) {
		sousa_->PlayAudio();
		isFrontBinder_ = !isFrontBinder_;
	}

	/// enter を押すか　game pad RB を押す
	if(isFrontBinder_) {
		lerpTime_ = std::min(lerpTime_ + WorldTime::DeltaTime(), maxLerpTime_);
	} else {
		lerpTime_ = std::max(lerpTime_ - WorldTime::DeltaTime(), 0.0f);
	}

	float lerpT = lerpTime_ / (maxLerpTime_ / 2.0f);
	if(lerpT <= 1.0f) {

		transform_.position = Vec3::Lerp(
			{ -6.0f, 0.6f, 1.0f },
			{ -4.2f, 2.0f, -2.35f },
			lerpT
		);

	} else {

		transform_.position = Vec3::Lerp(
			{ -4.2f, 2.0f, -2.35f },
			{ -3.6f, 0.6f, -4.7f },
			lerpT - 1.0f
		);

	}



	/// タイトルに戻る動作
	if(isFrontBinder_) {
		bool isTriggerKey = false;
		isTriggerKey |= Input::TriggerKey(KeyCode::Escape);
		isTriggerKey |= Input::TriggerPadButton(PadCode::LeftShoulder);

		if(isTriggerKey) {
			SceneManager::GetInstance()->SetNextScene(SCENE_ID::TITLE);
		}

	}

}

void GameOperationUI::Draw() {
	paper_->Draw(&transform_, &paperMaterial_);
	binder_->Draw(&transform_);
}

void GameOperationUI::Debug() {
	if(ImGui::TreeNodeEx("lerp", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &lerpTime_);
		ImGui::DragFloat("maxTime", &maxLerpTime_, 0.1f);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("paper", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::ColorEdit4("color", &paperColor_.x);
		if(ImGui::IsItemEdited()) {
			paperMaterial_.SetColor(paperColor_);
		}

		ImGui::TreePop();
	}
}

void GameOperationUI::SetIsFrontBinder(bool isFrontBinder) {
	isFrontBinder_ = isFrontBinder;
}
