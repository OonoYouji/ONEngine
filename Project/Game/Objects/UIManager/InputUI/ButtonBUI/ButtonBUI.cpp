#include "ButtonBUI.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ButtonBUI::ButtonBUI() {
	CreateTag(this);
}

ButtonBUI::~ButtonBUI() {}

void ButtonBUI::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("WeakAttackText.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ButtonBUI::Update() {
	ApplyVariables();
}

void ButtonBUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale",    pTransform_->scale);
}

void ButtonBUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}	

