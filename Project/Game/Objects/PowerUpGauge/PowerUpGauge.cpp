#include "PowerUpGauge.h"

#include <imgui.h>
#include "GraphicManager/TextureManager/TextureManager.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

#include "Objects/Player/Player.h"


PowerUpGauge::PowerUpGauge() {
	CreateTag(this);
}

PowerUpGauge::~PowerUpGauge() {}

void PowerUpGauge::Initialize() {
	drawLayerId = 1;

	TextureManager::GetInstance()->Load("bossHpGage", "bossHpGage.png");
	TextureManager::GetInstance()->Load("bossHpGageName", "bossHpGageName.png");

	mr_ = AddComponent<MeshRenderer>();
	mr_->SetModel("gage");
	mr_->SetMaterial("gage");
	mr_->GetMaterial()->SetIsLighting(false);

	pTransform_->position = { -1.0f, 0.0f, 0.0f };
	pTransform_->Update();

	uvPosition_ = {};
	uvScale_ = {0.0f , -0.35f};
}

void PowerUpGauge::Update() {
	float lerpT = (pPlayer_->GetPowerUpGauge() / pPlayer_->GetPowerUpGaugeMax());

	/// -0.715f ~ 0.0f.
	uvPosition_.y = std::lerp(0.4f, 0.0f, lerpT);


	Material* material = mr_->GetMaterial();
	material->SetPosition(uvPosition_);
	material->SetScale(uvScale_);
	material->UpdateMatrix();


}

void PowerUpGauge::Debug() {
	if(ImGui::TreeNodeEx("label", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("uv positon", &uvPosition_.x, 0.001f);
		ImGui::DragFloat2("uv scale", &uvScale_.x, 0.05f);

		ImGui::TreePop();
	}
}

void PowerUpGauge::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}
