#include "Stage.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Stage::Stage() {
	CreateTag(this);
}

Stage::~Stage() {}

void Stage::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Field");

	meshRenderer_->SetMaterial("FieldTex.png");
	meshRenderer_->SetUVScale(Vec2(3.0f, 3.0f));


	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void Stage::Update() {
	ApplyVariables();
}

void Stage::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
	vm->AddValue(groupName, "uvScale", uvScale_);
}

void Stage::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
	uvScale_              = vm->GetValue<Vec2>(groupName, "uvScale");


	meshRenderer_->SetUVScale(uvScale_);

}

