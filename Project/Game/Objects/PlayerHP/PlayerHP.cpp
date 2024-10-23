#include "PlayerHP.h"

/// engine
#include <imgui.h>
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


PlayerHP::PlayerHP() {
	CreateTag(this);
}

PlayerHP::~PlayerHP() {}

void PlayerHP::Initialize() {

	model_ = ModelManager::Load("playerHp");

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel(model_);

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(uvPosition_);

}

void PlayerHP::Update() {

	std::vector<Material>& materials = model_->GetMaterials();
	materials.front().SetPosition(uvPosition_);
	materials.front().UpdateMatrix();



}

void PlayerHP::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat2("uv position", &uvPosition_.x, 0.01f);

		ImGui::TreePop();
	}
}
