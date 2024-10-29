#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/AnimationRenderer/SkeletonRenderer.h"

void DemoObject::Initialize() {

	Model* model = ModelManager::Load("sneakWalk");

	skeleton_ = CreateSkeleton(model->GetRootNode());

	//AnimationRenderer* ar = AddComponent<AnimationRenderer>();
	//ar->SetModel("sneakWalk");
	//ar->LoadAnimation("sneakWalk");

	SkeletonRenderer* sr = AddComponent<SkeletonRenderer>();
	sr->SetSkeleton(&skeleton_);
}

void DemoObject::Update() {
	skeleton_.Update();
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("delete")) {
			Destory();
		}

		ImGui::TreePop();
	}
}
