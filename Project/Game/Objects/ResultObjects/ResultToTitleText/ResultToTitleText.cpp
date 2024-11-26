#include "ResultToTitleText.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ResultToTitleText::ResultToTitleText() {
	CreateTag(this);
}

ResultToTitleText::~ResultToTitleText() {}

void ResultToTitleText::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("Title.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void ResultToTitleText::Update() {
	ApplyVariables();

}


void ResultToTitleText::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ResultToTitleText::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}