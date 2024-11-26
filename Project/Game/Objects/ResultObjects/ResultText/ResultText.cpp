#include "ResultText.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ResultText::ResultText(const std::string& _filePath) : filePath_(_filePath) {
	CreateTag(this);
}

ResultText::~ResultText() {}

void ResultText::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture(filePath_);

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ResultText::Update() {
	ApplyVariables();
}

void ResultText::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ResultText::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}

