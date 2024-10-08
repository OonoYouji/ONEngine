#include "DemoObject.h"

#include "ImGuiManager/ImGuiManager.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"
//#include "ComponentManager/"

void DemoObject::Initialize() {
	//auto mr = AddComponent<MeshRenderer>();
	//mr->SetModel("Sphere");



}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
