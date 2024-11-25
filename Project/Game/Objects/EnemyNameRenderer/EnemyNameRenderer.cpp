#include "EnemyNameRenderer.h"

/// engine
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

EnemyNameRenderer::EnemyNameRenderer() {
    CreateTag(this);
}

EnemyNameRenderer::~EnemyNameRenderer() {}

void EnemyNameRenderer::Initialize() {


	drawLayerId = GAME_SCENE_LAYER_UI;


	VariableManager* vm = VariableManager::GetInstance();

	/// 値の追加
	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "scale", pTransform_->scale);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);
	vm->AddValue(groupName, "position", pTransform_->position);

	/// 読み込み
	vm->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", groupName
	);

	/// 適用
	ApplyVariables();



	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("GustavOfTitanNameText.png");
}

void EnemyNameRenderer::Update() {
	ApplyVariables();

}

void EnemyNameRenderer::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
	pTransform_->rotate = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
}

