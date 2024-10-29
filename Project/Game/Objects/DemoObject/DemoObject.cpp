#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

void DemoObject::Initialize() {

	//AnimationRenderer* ar = AddComponent<AnimationRenderer>();
	//ar->SetModel("AnimatedCube");
	//ar->LoadAnimation("AnimatedCube");

	particleField_.reset(new ParticleField);
	particleField_->Initialize();
	particleField_->SetUpdateFunction([](Particle* particle) {
		Transform* transform = particle->GetTransform();
		transform->position.y += 0.025f;
	});

	particleField_->SetMin({ -1, -1, -1 });
	particleField_->SetMax({  1,  1, 20 });

	ParticleSystem* ps = AddComponent<ParticleSystem>(12, "Sphere");
	ps->AddField(particleField_.get());

	ps->SetPartilceUpdateFunction([](Particle* particle) {
		Transform* transform = particle->GetTransform();

		transform->position.z += 0.025f;
	});

}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("delete")) {
			Destory();
		}

		ImGui::TreePop();
	}
}
