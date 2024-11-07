#include "ScoreBoard.h"

/// engine
#include "VariableManager/VariableManager.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"


ScoreBoard::ScoreBoard() {
	CreateTag(this);
}

ScoreBoard::~ScoreBoard() {}

void ScoreBoard::Initialize() {

	/// 表示するレイヤー層
	drawLayerId = SCENE_GAME_LAYER_UI;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("ScoreBoard.png");
	
	
	/// ---------------------------------------------------
	/// jsonで外部保存
	/// ---------------------------------------------------

	VariableManager* vm = VariableManager::GetInstance();

	const std::string& groupName = GetTag();
	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->position);

	ApplyVariables();
}

void ScoreBoard::Update() {

}

void ScoreBoard::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");

}

