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

	AnimationRenderer* ar = AddComponent<AnimationRenderer>();
	ar->SetModel("Sphere");
	ar->LoadAnimation("AnimatedCube");

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
