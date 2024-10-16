#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

void DemoObject::Initialize() {
	//auto mr = AddComponent<MeshRenderer>();
	//mr->SetModel("Sphere");

	//auto ps = AddComponent<ParticleSystem>(12, "Board");

	//ps->SetPartilceUpdateFunction([](Particle* par) {
	//	Transform* transform = par->GetTransform();
	//	transform->position.y += Time::DeltaTime();
	//});

	auto mir = AddComponent<MeshInstancingRenderer>(12);
	mir->AddTransform(pTranform_);

}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
