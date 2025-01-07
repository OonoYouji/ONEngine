#include "PlayerOperationGUI.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// user
#include "Scenes/Scene_Game.h"


PlayerOperationGUI::PlayerOperationGUI() {
	CreateTag(this);
}

PlayerOperationGUI::~PlayerOperationGUI() {}

void PlayerOperationGUI::Initialize() {


	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("OperationGUI.png");

	drawLayerId = GAME_SCENE_LAYER_UI;

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects/", GetTag());
	ApplyVariables();
}

void PlayerOperationGUI::Update() {
	ApplyVariables();
}


void PlayerOperationGUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void PlayerOperationGUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

