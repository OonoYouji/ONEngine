#define NOMINMAX
#include "GameStartUI.h"

#include <SceneManager.h>
#include <ImGuiManager.h>
#include <WorldTime.h>
#include <Input.h>

#include <numbers>




void GameStartUI::Initialize() {
	start_.reset(new Sprite);
	start_->Initialize("start", "start.png");

	end_.reset(new Sprite);
	end_->Initialize("end", "end.png");

	for(auto& arrow : arrows_) {
		arrow.reset(new Sprite);
		arrow->Initialize("arrow", "arrow.png");
	}

	offset_ = { 640.0f, 500.0f, 0.0f };
	distance_ = { 0.0f, 40.0f, 0.0f };
	spriteSize_ = { 128.0f, 32.0f };

	arrowSpriteSize_ = { 20.0f, 20.0f };
	arrowOffset_ = 180.0f;
	arrows_[1]->SetAngle(1.0f * std::numbers::pi_v<float>);

	spriteColor_ = { 0,0,0,0 };

	currentSelectMode_ = 1; /// start で初期化
}

void GameStartUI::Update() {

	/// 選択しているモードを変える  //// TODO : コントローラーに対応させる
	if(Input::TriggerKey(KeyCode::DownArrow)) {
		currentSelectMode_ = std::max(currentSelectMode_ - 1, 0);
	} else if(Input::TriggerKey(KeyCode::UpArrow)) {
		currentSelectMode_ = std::min(currentSelectMode_ + 1, 1);
	}

	/// ////////////////////////////////////////////////////////////////////////
	/// ゲームシーンに遷移する || ゲームを終わる
	/// ////////////////////////////////////////////////////////////////////////
	if(Input::TriggerKey(KeyCode::Space)) {
		if(currentSelectMode_) {
			SceneManager::GetInstance()->SetNextScene(GAME);
		}
	}

	/// 文字の色をイージングする
	animationTime_ += WorldTime::DeltaTime();
	if(animationTime_ > 5.0f) {
		spriteColor_ = Vec4(1, 1, 1, 1) * (std::cos(animationTime_ * animationSpeed_) * 0.25f + 0.75f);
	} else {
		spriteColor_ = Vec4::Lerp({ 0,0,0,0 }, { 1,1,1,1 }, animationTime_ / 5.0f);
	}
	spriteColor_.w = 1.0f;


	SettingSprites();
	RecalculateArrowPosition();

}

void GameStartUI::FrontSpriteDraw() {
	start_->Draw();
	end_->Draw();
	for(auto& arrow : arrows_) {
		arrow->Draw();
	}
}

void GameStartUI::Debug() {
	if(ImGui::TreeNodeEx("sprite", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("offset", &offset_.x, 1.0f);
		ImGui::DragFloat2("distance", &distance_.x, 1.0f);

		ImGui::DragFloat2("size", &spriteSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &spriteColor_.x);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("arrow", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("offset", &arrowOffset_, 1.0f);
		ImGui::DragFloat2("size", &arrowSpriteSize_.x, 1.0f);

		ImGui::TreePop();
	}

	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &animationSpeed_, 0.1f);

		ImGui::TreePop();
	}

}

void GameStartUI::SettingSprites() {
	/// 値のセッティング
	start_->SetPos(offset_ - distance_);
	end_->SetPos(offset_ + distance_);

	start_->SetSize(spriteSize_);
	end_->SetSize(spriteSize_);

	start_->SetColor(spriteColor_);
	end_->SetColor(spriteColor_);

	for(auto& arrow : arrows_) {
		arrow->SetSize(arrowSpriteSize_);
		arrow->SetColor(spriteColor_);
	}


}

void GameStartUI::RecalculateArrowPosition() {
	Vec3 position = {};
	if(currentSelectMode_) { /// start mode
		position = offset_ - distance_;
	} else { /// end mode
		position = offset_ + distance_;
	}

	position.x += arrowOffset_;
	arrows_[0]->SetPos(position);
	position.x -= arrowOffset_ * 2.0f;
	arrows_[1]->SetPos(position);
}
