#include "TitleExitText.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// title
#include "Scenes/Scene_Title.h"


TitleExitText::TitleExitText() {
	CreateTag(this);
}

TitleExitText::~TitleExitText() {}

void TitleExitText::Initialize() {
	drawLayerId = TITLE_LAYER_UI;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("Exit.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void TitleExitText::Update() {
	ApplyVariables();

	/// アスペクト比を維持する
	pTransform_->scale.x = pTransform_->scale.y * 8.0f;
}


void TitleExitText::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);
	vm->AddValue(groupName, "scale", pTransform_->scale);
}

void TitleExitText::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}
