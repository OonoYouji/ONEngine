#include "PlayerHP.h"

/// engine
#include <imgui.h>
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


PlayerHP::PlayerHP() {
	CreateTag(this);
}

PlayerHP::~PlayerHP() {}

void PlayerHP::Initialize() {

	TextureManager::GetInstance()->Load("playerHpNum", "playerHpNum.png");
	model_ = ModelManager::Load("playerHp");

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model_);
	
	numberRenderer_ = AddComponent<MeshRenderer>();
	numberRenderer_->SetModel(model_);
	numberRenderer_->SetMaterial("playerHpNum");

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(gaugeUVPosition_);


	numberUVPosition_ = { 0.0f, 0.0f };
	numberUVScale_ = { 1, 1 };
}

void PlayerHP::Update() {

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(gaugeUVPosition_);
	materials.front().UpdateMatrix();

	Material* material = numberRenderer_->GetMaterial();
	material->SetPosition(numberUVPosition_);
	material->SetScale(numberUVScale_);
	material->UpdateMatrix();

}

void PlayerHP::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("gauge uv position",  &gaugeUVPosition_.x,  0.01f);
		ImGui::DragFloat2("number uv position", &numberUVPosition_.x, 0.01f);
		ImGui::DragFloat2("number uv scale",    &numberUVScale_.x,    0.01f);

		ImGui::TreePop();
	}
}
