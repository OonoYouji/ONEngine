#include "ButtonLTUI.h"


#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ButtonLTUI::ButtonLTUI() {
    CreateTag(this);
}

ButtonLTUI::~ButtonLTUI() {}

void ButtonLTUI::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("LockOnText.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ButtonLTUI::Update() {
	ApplyVariables();
}

void ButtonLTUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ButtonLTUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}

