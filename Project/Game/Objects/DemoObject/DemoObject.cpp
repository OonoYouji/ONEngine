#include "DemoObject.h"

/// engine
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/Collider/CapsuleCollider.h"

void DemoObject::Initialize() {
	CapsuleCollider* cc = AddComponent<CapsuleCollider>();
	
	for(size_t i = 0; i < 2; ++i) {
		transformArray_[i].Initialize();
		transformArray_[i].SetName("Transform##" + std::to_string(i));
	}

	transformArray_[0].position = { 1.0f, 0.0f, 0.0f};
	transformArray_[1].position = { -1.0f, 0.0f, 0.0f};
	
	cc->SetTransformArray({ &transformArray_[0], &transformArray_[1] });
}

void DemoObject::Update() {
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(auto& transform : transformArray_) {
			transform.Debug();
		}

		ImGui::TreePop();
	}
}
