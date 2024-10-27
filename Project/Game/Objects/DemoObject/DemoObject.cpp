#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"

void DemoObject::Initialize() {

	auto mr = AddComponent<MeshRenderer>();
	mr->SetModel("AnimatedCube");

}

void DemoObject::Update() {}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}
