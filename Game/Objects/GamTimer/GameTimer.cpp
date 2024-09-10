#include "GameTimer.h"

#include <ImGuiManager.h>
#include <WorldTime.h>


void GameTimer::Initialize() {
	sprite_.reset(new Sprite);
	sprite_->Initialize("white2x2", "white2x2.png");
	transform_.position = Vec3(640, 360, 0);
	transform_.scale = Vec3(1280, 720, 1) / 2.0f;

	currentTime_ = 0.0f;
}

void GameTimer::Update() {

	currentTime_ += WorldTime::DeltaTime();
	float t = currentTime_ / maxTime_;
	transform_.position.y = std::lerp(720.0f, 1440.0f, t);

	UpdateMatrix();
	sprite_->SetPos(GetPosition());
	sprite_->SetSize({ transform_.scale.x, transform_.scale.y });
	sprite_->SetColor(color_);
}

void GameTimer::BackSpriteDraw() {
	sprite_->Draw(1);
}

void GameTimer::Debug() {
	if(ImGui::TreeNodeEx("material", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit4("color", &color_.x);
		ImGui::TreePop();
	}
	
	if(ImGui::TreeNodeEx("time", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("max time", &maxTime_, 0.1f);
		ImGui::DragFloat("current time", &currentTime_, 0.1f);
		ImGui::TreePop();
	}

}
