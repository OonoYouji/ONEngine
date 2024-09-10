#include "GameTitle.h"

#include <ImGuiManager.h>
#include <CameraManager.h>

#include <WorldTime.h>
#include <Input.h>

#include <GameStartUI/GameStartUI.h>


void GameTitle::Initialize() {
	title_.reset(new Sprite);
	title_->Initialize("title", "title.png");

	titlePosition_ = { 640, 250, 0 };
	titleSize_ = { 640, 360 };
	titleColor_ = { 0,0,0,1 };

	title_->SetPos(titlePosition_);
	title_->SetSize(titleSize_);
	title_->SetColor(titleColor_);


	/// カメラの座標を設定
	BaseCamera* camera =
		CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
		{ {0.0f, 0.3f, -7.6f}, { 0.0f, 0.0f, 0.0f } },
		0.0f
	);

}

void GameTitle::Update() {

	/// せっかち用のショートカット
	if(Input::TriggerKey(KeyCode::Space)) {
		SettingCameraAnimation();
		CreateStartUI();
	}


	animationTime_ += WorldTime::DeltaTime();
	if(animationTime_ > 5.0f) {
		SettingCameraAnimation();
	}

	if(animationTime_ <= 5.0f) {
		titleColor_ = Vec4::Lerp({ 0,0,0,1 }, { 1,1,1,1 }, animationTime_ / 5.0f);
	} else {
		titleColor_ = { 1,1,1,1 };
		//titleColor_ = Vec4(1, 1, 1, 1) * (std::sin(animationTime_ * animationSpeed_) * 0.5f + 0.5f);
		//titleColor_.w = 1.0f; /// 固定

		CreateStartUI();
	}

	title_->SetPos(titlePosition_);
	title_->SetSize(titleSize_);
	title_->SetColor(titleColor_);
}

void GameTitle::FrontSpriteDraw() {
	title_->Draw();
}

void GameTitle::Debug() {
	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("time", &animationTime_, 1.0f);
		ImGui::DragFloat("speed", &animationSpeed_, 1.0f);
		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("title", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat2("position", &titlePosition_.x, 1.0f);
		ImGui::DragFloat2("size", &titleSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &titleColor_.x);
		ImGui::TreePop();
	}

}



void GameTitle::SettingCameraAnimation() {
	if(!isAnimationStarted_) {
		isAnimationStarted_ = true;

		BaseCamera* camera =
			CameraManager::GetInstance()->GetCamera("GameCamera");
		camera->SetMove(
			{ {0.0f, 0.2f, -7.6f}, { 0.0f, 0.0f, 0.0f } },
			{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
			0.5f
		);
	}
}

void GameTitle::CreateStartUI() {
	if(!isCreatedStartUI_) {
		isCreatedStartUI_ = true;
		(new GameStartUI)->Initialize();
	}
}
