#include "ResultArrow.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ResultArrow::ResultArrow() {
	CreateTag(this);
}

ResultArrow::~ResultArrow() {}

void ResultArrow::Initialize() {
	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("Arrow.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void ResultArrow::Update() {
	ApplyVariables();

}


void ResultArrow::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ResultArrow::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}