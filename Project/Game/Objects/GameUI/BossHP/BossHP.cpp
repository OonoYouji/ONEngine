#include "BossHP.h"

#include <numbers>

#include <imgui.h>

#include "GraphicManager/TextureManager/TextureManager.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

#include "Objects/Boss/Boss.h"

BossHP::BossHP() {
	CreateTag(this);
}

BossHP::~BossHP() {}

void BossHP::Initialize() {

	TextureManager::GetInstance()->Load("bossHpGage", "bossHpGage.png");

	mr_ = AddComponent<MeshRenderer>();
	mr_->SetModel("gage");
	mr_->SetMaterial("bossHpGage");

	pTransform_->rotate.z = std::numbers::pi_v<float>;
	pTransform_->position = { 1.0f, 0.0f, 0.0f };
	pTransform_->Update();

	uvPosition_ = {};
	uvScale_ = { 0.0f , -0.35f };
}

void BossHP::Update() {
	float lerpT = (pBoss_->GetHP() / pBoss_->GetMaxHP());

	///// -0.715f ~ 0.0f.
	uvPosition_.y = std::lerp(0.4f, 0.0f, lerpT);


	Material* material = mr_->GetMaterial();
	material->SetPosition(uvPosition_);
	material->SetScale(uvScale_);
	material->UpdateMatrix();

}

void BossHP::Debug() {
	if(ImGui::TreeNodeEx("label", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("uv positon", &uvPosition_.x, 0.001f);
		ImGui::DragFloat2("uv scale", &uvScale_.x, 0.05f);

		ImGui::TreePop();
	}
}

void BossHP::SetBoss(Boss* _boss) {
	pBoss_ = _boss;
}
