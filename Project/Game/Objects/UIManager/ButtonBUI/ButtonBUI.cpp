#include "ButtonBUI.h"

#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ButtonBUI::ButtonBUI() {
	CreateTag(this);
}

ButtonBUI::~ButtonBUI() {}

void ButtonBUI::Initialize() {

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("buttonA.png");

	AddVariables();
	//VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resoures/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ButtonBUI::Update() {
	ApplyVariables();
}

void ButtonBUI::AddVariables() {
	VariableManager::GetInstance();

}

void ButtonBUI::ApplyVariables() {

}

