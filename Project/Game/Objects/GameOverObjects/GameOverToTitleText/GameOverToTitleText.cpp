#include "GameOverToTitleText.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

GameOverToTitleText::GameOverToTitleText() {
	CreateTag(this);
}

GameOverToTitleText::~GameOverToTitleText() {}

void GameOverToTitleText::Initialize() {
	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("Title.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void GameOverToTitleText::Update() {
	ApplyVariables();

}



void GameOverToTitleText::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void GameOverToTitleText::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}