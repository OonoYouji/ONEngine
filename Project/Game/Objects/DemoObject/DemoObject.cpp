#include "DemoObject.h"

#include "ImGuiManager/ImGuiManager.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

void DemoObject::Initialize() {
	//auto mr = AddComponent<MeshRenderer>();
	//mr->SetModel("Sphere");

	auto ps = AddComponent<ParticleSystem>(12, "Board");

	ps->SetPartilceUpdateFunction([](Particle* par) {

	});


}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
