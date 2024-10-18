#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


void DemoObject::Initialize() {
	//auto mr = AddComponent<MeshRenderer>();
	//mr->SetModel("Sphere");

	//auto ps = AddComponent<ParticleSystem>(12, "Board");

	//ps->SetPartilceUpdateFunction([](Particle* par) {
	//	Transform* transform = par->GetTransform();
	//	transform->position.y += Time::DeltaTime();
	//});

	//ps->SetParticleEmitterFlags(PARTICLE_EMITTER_NOTIME);
	//ps->SetBoxEmitterMinMax(-Vec3::kOne, Vec3::kOne);

	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("uvChecker");
	spriteRenderer->SetColor(Vec4(0, 0, 0, 1));

}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}
