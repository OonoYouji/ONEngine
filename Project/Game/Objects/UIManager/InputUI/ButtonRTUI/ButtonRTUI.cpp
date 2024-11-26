#include "ButtonRTUI.h"


#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ButtonRTUI::ButtonRTUI() {
	CreateTag(this);
}

ButtonRTUI::~ButtonRTUI() {}

void ButtonRTUI::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("AvoidanceText.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ButtonRTUI::Update() {
	ApplyVariables();
}

void ButtonRTUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ButtonRTUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}


