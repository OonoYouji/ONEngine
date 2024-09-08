#include "PlayerHP.h"

#include <ImGuiManager.h>

void PlayerHP::Initialize() {
	maxHP_ = 3;
	currentHP_ = maxHP_;

	offset_ = Vec3(150.0f, 100.0f, 0.0f);
	distance_ = 140.0f;

	hpSprites_.resize(maxHP_);
	for(auto& sprite : hpSprites_) {
		sprite.reset(new Sprite);
		sprite->Initialize("Heart1", "Heart1.png");
		sprite->SetSize({ 64,64 });
	}

}

void PlayerHP::Update() {
	/// 座標初期化
	for(uint32_t i = 0U; i < static_cast<uint32_t>(hpSprites_.size()); ++i) {
		Vec3 position = {
			distance_ * i,
			0.0f,
			0.0f
		};
		position += offset_;
		hpSprites_[i]->SetPos(position);
	}
}

void PlayerHP::FrontSpriteDraw() {
	for(auto& sprite : hpSprites_) {
		sprite->Draw(2);
	}
}

void PlayerHP::Debug() {
	if(ImGui::TreeNodeEx("status", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("offset", &offset_.x);
		ImGui::DragFloat("distance", &distance_);
		ImGui::DragInt("currentHP", reinterpret_cast<int*>(&currentHP_), 0);
		ImGui::DragInt("maxHP", reinterpret_cast<int*>(&maxHP_), 0);

		ImGui::TreePop();
	}
}
