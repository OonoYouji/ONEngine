#include "TitleTextRenderer.h"

#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Scenes/Scene_Title.h"


TitleTextRenderer::TitleTextRenderer() {
	CreateTag(this);
}

TitleTextRenderer::~TitleTextRenderer() {}

void TitleTextRenderer::Initialize() {

	drawLayerId = TITLE_LAYER_UI;


	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("TitleText.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();

}

void TitleTextRenderer::Update() {
	ApplyVariables();

}

void TitleTextRenderer::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "rotate",   pTransform_->rotate);
	vm->AddValue(groupName, "scale",    pTransform_->scale);

}

void TitleTextRenderer::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
}

