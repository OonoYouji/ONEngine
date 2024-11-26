#include "InputUI.h"

#include "VariableManager/VariableManager.h"

#include "ButtonAUI/ButtonAUI.h"
#include "ButtonBUI/ButtonBUI.h"

InputUI::InputUI() {
	CreateTag(this);
}

InputUI::~InputUI() {}

void InputUI::Initialize() {

	uis_.push_back(new ButtonAUI());
	uis_.push_back(new ButtonBUI());


	for(auto& ui : uis_) {
		ui->Initialize();
		ui->SetParent(pTransform_);
	}


	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();

}

void InputUI::Update() {
	ApplyVariables();

	for(auto& ui : uis_) {
		ui->drawLayerId = drawLayerId;
	}

}


void InputUI::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void InputUI::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}



