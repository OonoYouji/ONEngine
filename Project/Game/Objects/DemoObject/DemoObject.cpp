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

	for(size_t i = 0; i < 12; ++i) {
		transformArray_.push_back(std::make_unique<Transform>());
		Transform* transform = transformArray_.back().get();
		transform->position = {
			0.0f, 0.0f,
			i * 1.0f
		};

		transform->UpdateMatrix(false);
		mir->AddTransform(transform);
	}

}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
