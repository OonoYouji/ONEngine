#include "EnemyHPFrame.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

EnemyHPFrame::EnemyHPFrame() {
	CreateTag(this);
}

EnemyHPFrame::~EnemyHPFrame() {}

void EnemyHPFrame::Initialize() {

	/// 描画するlayer : ui layerに描画する
	drawLayerId = GAME_SCENE_LAYER_UI;

	/// ---------------------------------------------------
	/// 値の外部保存と読み込み
	/// ---------------------------------------------------

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


	/// ---------------------------------------------------
	/// スプライトの初期化
	/// ---------------------------------------------------

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("white2x2.png");

}

void EnemyHPFrame::Update() {
	/// jsonの値を適用
	ApplyVariables();
}

void EnemyHPFrame::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
	pTransform_->rotate = vm->GetValue<Vec3>(groupName, "rotate");
	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
}
