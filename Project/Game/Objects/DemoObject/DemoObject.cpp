#include "DemoObject.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void DemoObject::Initialize() {
	renderer_ = AddComponent<MeshRenderer>();
	renderer_->SetModel("Sphere");
	renderer_->SetMaterial("uvChecker.png");
}

void DemoObject::Update() {
	

}

void DemoObject::Debug() {

	ImGui::DragFloat("shininess", &shininess, 0.01f);
	renderer_->SetShiniess(shininess);

	
}

