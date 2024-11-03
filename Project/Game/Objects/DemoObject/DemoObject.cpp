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


void DemoObject::Initialize() {

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "isAlive", isAlive_);
	vm->AddValue(groupName, "speed", speed_);
	vm->AddValue(groupName, "id", id_);
	vm->AddValue(groupName, "size", size_);
	vm->AddValue(groupName, "position", position_);
	vm->AddValue(groupName, "color", color_);
}

void DemoObject::Update() {
}

void DemoObject::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("load")) {
			VariableManager* vm = VariableManager::GetInstance();
			vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
		}


		ImGui::TreePop();
	}
}

void DemoObject2::Initialize() {

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "isAlive", isAlive_);
	vm->AddValue(groupName, "speed", speed_);
	vm->AddValue(groupName, "id", id_);
	vm->AddValue(groupName, "size", size_);
	vm->AddValue(groupName, "position", position_);
	vm->AddValue(groupName, "color", color_);
}

void DemoObject2::Update() {
}

void DemoObject2::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::Button("load")) {
			VariableManager* vm = VariableManager::GetInstance();
			vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
		}


		ImGui::TreePop();
	}
}
