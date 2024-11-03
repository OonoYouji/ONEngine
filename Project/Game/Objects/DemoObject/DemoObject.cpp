#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/AnimationRenderer/SkeletonRenderer.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

void DemoObject::Initialize() {
	auto sprite = AddComponent<SpriteRenderer>();
	sprite->SetTexture("light.png");
}

void DemoObject::Update() {
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {


		ImGui::TreePop();
	}
}

void DemoObject2::Initialize() {
	auto sprite = AddComponent<SpriteRenderer>();
	sprite->SetTexture("uvChecker.png");
}

void DemoObject2::Update() {
}

void DemoObject2::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {



		ImGui::TreePop();
	}
}
