#include "ResultToTitle.h"


#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


ResultToTitle::ResultToTitle() {
	CreateTag(this);
}

ResultToTitle::~ResultToTitle() {}

void ResultToTitle::Initialize() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("ToTitle.png");


	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	ApplyVariables();
}

void ResultToTitle::Update() {
	ApplyVariables();

}

void ResultToTitle::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

