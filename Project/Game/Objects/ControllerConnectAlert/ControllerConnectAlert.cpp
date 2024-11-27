#include "ControllerConnectAlert.h"

#include "Input/Input.h"
#include "VariableManager/VariableManager.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Scenes/Scene_Title.h"

ControllerConnectAlert::ControllerConnectAlert() {
    CreateTag(this);
}

ControllerConnectAlert::~ControllerConnectAlert() {}


void ControllerConnectAlert::Initialize() {

	drawLayerId = TITLE_LAYER_UI;

	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("controller.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void ControllerConnectAlert::Update() {
	ApplyVariables();

	if(Input::GamepadConnected(0)) {
		renderer_->isActive = false;
	} else {
		renderer_->isActive = true;
	}
}

void ControllerConnectAlert::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void ControllerConnectAlert::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}

