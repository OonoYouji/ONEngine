#include "TitleSelectorUI.h"

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Scenes/Scene_Title.h"

TitleSelectorUI::TitleSelectorUI() {
	CreateTag(this);
}

TitleSelectorUI::~TitleSelectorUI() {}

void TitleSelectorUI::Initialize() {

	drawLayerId = TITLE_LAYER_UI;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("StartButton.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
	
}

void TitleSelectorUI::Update() {
	ApplyVariables();

	if(Input::GamepadConnected(0)) {
		spriteRenderer_->SetTexture("StartButton.png");
		pTransform_->scale = gamepadUIScale_;
	} else {
		spriteRenderer_->SetTexture("SpaceKeyUI.png");
		pTransform_->scale = keyboardUIScale_;
	}

}

void TitleSelectorUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate",   pTransform_->rotate);
	vm->AddValue(groupName, "scale",    pTransform_->scale);
	vm->AddValue(groupName, "gamepadUIScale", gamepadUIScale_);
	vm->AddValue(groupName, "keyboardUIScale", keyboardUIScale_);
}

void TitleSelectorUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");

	gamepadUIScale_  = vm->GetValue<Vec3>(groupName, "gamepadUIScale");
	keyboardUIScale_ = vm->GetValue<Vec3>(groupName, "keyboardUIScale");
}

