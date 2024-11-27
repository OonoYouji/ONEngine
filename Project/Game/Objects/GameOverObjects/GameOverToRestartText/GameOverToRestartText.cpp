#include "GameOverToRestartText.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

GameOverToRestartText::GameOverToRestartText() {
	CreateTag(this);
}

GameOverToRestartText::~GameOverToRestartText() {}

void GameOverToRestartText::Initialize() {
	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("restart.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void GameOverToRestartText::Update() {
	ApplyVariables();

}



void GameOverToRestartText::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void GameOverToRestartText::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}