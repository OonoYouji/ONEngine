#include "Background.h"

#include <ImGuiManager.h>


void Background::Initialize() {
	sprite_.reset(new Sprite);
	sprite_->Initialize("white2x2", "white2x2s.png");
	transform_.position = Vec3(640, 360, 0);
	transform_.scale = Vec3(1280, 720, 1);
	sprite_->SetPos(GetPosition());
	sprite_->SetSize({ transform_.scale.x, transform_.scale.y });
	sprite_->SetColor({ 0,0,0,1 });
}

void Background::Update() {
	UpdateMatrix();
	sprite_->SetPos(GetPosition());
	sprite_->SetSize({ transform_.scale.x, transform_.scale.y });
	sprite_->SetColor(color);
	sprite_->SetAngle(angle_);
}

void Background::BackSpriteDraw() {
	sprite_->Draw(0);
}

void Background::Debug() {
	if(ImGui::TreeNodeEx("", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("angle", &angle_);
		ImGui::ColorEdit4("color", &color.x);
		ImGui::TreePop();
	}
}
