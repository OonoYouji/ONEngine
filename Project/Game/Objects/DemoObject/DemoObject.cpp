#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/Collider/CapsuleCollider.h"
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

void DemoObject::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("axis");
	meshRenderer_->SetMaterial("white2x2");

	CapsuleCollider* cc = AddComponent<CapsuleCollider>();
	
	positionArray_[0] = {  1.0f, 0.0f, 0.0f };
	positionArray_[1] = { -1.0f, 0.0f, 0.0f };
	
	cc->SetPositionArray({ &positionArray_[0], &positionArray_[1] });
}

void DemoObject::Update() {
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat3("position1", &positionArray_[0].x, 0.01f);
		ImGui::DragFloat3("position2", &positionArray_[1].x, 0.01f);

		ImGui::TreePop();
	}
}

void DemoObject::OnCollisionStay(BaseGameObject* const _collision) {
	meshRenderer_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
}

void DemoObject::OnCollisionEnter(BaseGameObject* const _collision) {
	meshRenderer_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
}

void DemoObject::OnCollisionExit(BaseGameObject* const _collision) {
	meshRenderer_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}
