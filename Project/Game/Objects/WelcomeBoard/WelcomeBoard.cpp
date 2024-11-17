#include "WelcomeBoard.h"

#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

WelcomeBoard::WelcomeBoard() {
    CreateTag(this);
}

WelcomeBoard::~WelcomeBoard() {}

void WelcomeBoard::Initialize() {
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("WelcomeBoard");

	VariableManager* vm = VariableManager::GetInstance();

	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	ApplyVariables();
}

void WelcomeBoard::Update() {
	ApplyVariables();

}

void WelcomeBoard::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}

