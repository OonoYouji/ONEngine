#include "DemoObject.h"

#include <ImGuiManager.h>
#include <Component/MeshRenderer/MeshRenderer.h>

void DemoObject::Initialize() {
	auto mr = AddComponent<MeshRenderer>();
	mr->SetModel("Sphere");
}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
