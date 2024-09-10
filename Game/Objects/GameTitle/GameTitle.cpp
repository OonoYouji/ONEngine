#include "GameTitle.h"

#include <ImGuiManager.h>

void GameTitle::Initialize() {
	title_.reset(new Sprite);
	title_->Initialize("title", "title.png");

	titlePosition_ = { 640, 320, 0 };
	titleSize_ = { 640, 360};
	titleColor_ = { 1,1,1,1 };

	title_->SetPos(titlePosition_);
	title_->SetSize(titleSize_);
	title_->SetColor(titleColor_);


}

void GameTitle::Update() {
	title_->SetPos(titlePosition_);
	title_->SetSize(titleSize_);
	title_->SetColor(titleColor_);
}

void GameTitle::FrontSpriteDraw() {
	title_->Draw();
}

void GameTitle::Debug() {
	if(ImGui::TreeNodeEx("title", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat2("position", &titlePosition_.x, 1.0f);
		ImGui::DragFloat2("size", &titleSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &titleColor_.x);
		ImGui::TreePop();
	}
}
