#include "TitleSprite.h"

/// engine
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"
#include "VariableManager/VariableManager.h"


TitleSprite::TitleSprite() {
	CreateTag(this);
}

TitleSprite::~TitleSprite() {}

void TitleSprite::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("Title.png");

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", GetTag());
	ApplyVariables();
}

void TitleSprite::Update() {
	ApplyVariables();

}

void TitleSprite::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);
}

void TitleSprite::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	
	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");
	pTransform_->rotate   = vm->GetValue<Vec3>(groupName, "rotate");
}

