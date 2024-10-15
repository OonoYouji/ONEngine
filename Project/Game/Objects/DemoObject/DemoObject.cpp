#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"


void DemoObject::Initialize() {
	//auto mr = AddComponent<MeshRenderer>();
	//mr->SetModel("Sphere");

	auto ps = AddComponent<ParticleSystem>(12, "Board");

	ps->SetPartilceUpdateFunction([](Particle* par) {
		Transform* transform = par->GetTransform();
		transform->position.y += Time::DeltaTime();
	});


}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
