#include "GameStartUI.h"

#include <ImGuiManager.h>
#include <WorldTime.h>

void GameStartUI::Initialize() {
	start_.reset(new Sprite);
	start_->Initialize("uvChecker", "uvChecker.png");

	end_.reset(new Sprite);
	end_->Initialize("uvChecker", "uvChecker.png");

	offset_ = { 640.0f, 460.0f, 0.0f };
	distance_ = { 200.0f, 0.0f, 0.0f };
	spriteSize_ = { 100.0f, 100.0f };

	spriteColor_ = { 0,0,0,0 };

}

void GameStartUI::Update() {

	animationTime_ += WorldTime::DeltaTime();
	if(animationTime_ > 5.0f) {
		spriteColor_ = Vec4(1, 1, 1, 1) * (std::cos(animationTime_ * animationSpeed_) * 0.25f + 0.75f);
	} else {
		spriteColor_ = Vec4::Lerp({ 0,0,0,0 }, { 1,1,1,1 }, animationTime_ / 5.0f);
	}
	spriteColor_.w = 1.0f;


	start_->SetPos(offset_ - distance_);
	end_->SetPos(offset_ + distance_);

	start_->SetSize(spriteSize_);
	end_->SetSize(spriteSize_);

	start_->SetColor(spriteColor_);
	end_->SetColor(spriteColor_);

}

void GameStartUI::FrontSpriteDraw() {
	start_->Draw();
	end_->Draw();
}

void GameStartUI::Debug() {
	if(ImGui::TreeNodeEx("sprite", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("offset", &offset_.x, 1.0f);
		ImGui::DragFloat2("distance", &distance_.x, 1.0f);

		ImGui::DragFloat2("size", &spriteSize_.x, 1.0f);
		ImGui::ColorEdit4("color", &spriteColor_.x);

		ImGui::TreePop();
	}


	if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("time", &animationTime_, 0.0f);
		ImGui::DragFloat("speed", &animationSpeed_, 0.1f);

		ImGui::TreePop();
	}

}
