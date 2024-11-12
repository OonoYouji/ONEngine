#include "ResultSprite.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ResultSprite::ResultSprite() {
	CreateTag(this);
}

ResultSprite::~ResultSprite() {}

void ResultSprite::Initialize() {

	SpriteRenderer* spriteRenderer = AddComponent<SpriteRenderer>();
	spriteRenderer->SetTexture("GameResult.png");

	/// 値の保存と読み込み
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale",    pTransform_->scale);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	ApplyVariables();
}

void ResultSprite::Update() {
	ApplyVariables();
}

void ResultSprite::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
}

