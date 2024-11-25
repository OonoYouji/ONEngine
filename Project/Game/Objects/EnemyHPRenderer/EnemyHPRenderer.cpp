#include "EnemyHPRenderer.h"

/// std
#include <numbers>

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

/// objects
#include "Objects/Enemy/Enemy.h"
#include "EnemyHPFrame.h"



EnemyHPRenderer::EnemyHPRenderer() {
	CreateTag(this);
}

EnemyHPRenderer::~EnemyHPRenderer() {}

void EnemyHPRenderer::Initialize() {

	/// ui layerに表示
	drawLayerId = GAME_SCENE_LAYER_UI;


	/// ---------------------------------------------------
	/// 値の外部保存と読み込み
	/// ---------------------------------------------------

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);
	vm->AddValue(groupName, "rotate", pTransform_->rotate);
	vm->AddValue(groupName, "uvAnchor", uvAnchor_);

	/// jsonの読み込み
	vm->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", groupName
	);

	ApplyVariables();


	/// ---------------------------------------------------
	/// スプライト描画の初期化
	/// ---------------------------------------------------

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("uvChecker");
	spriteRenderer_->SetAnchor(uvAnchor_);

	maxScale_ = GetScale();
	spriteRenderer_->SetUVRotate(std::numbers::pi_v<float>);

	/// objects create
	hpFrame_ = new EnemyHPFrame();
	hpFrame_->Initialize();
}

void EnemyHPRenderer::Update() {

	ApplyVariables();

	/// hpによって大きさを変える
	float lerpT = pEnemy_->GetHP() / pEnemy_->GetMaxHP();
	pTransform_->scale = Vec3::Lerp(
		{ 0.0f, maxScale_.y, 1.0f, }, maxScale_, lerpT
	);

}

void EnemyHPRenderer::SetEnemy(Enemy* _enemy) {
	pEnemy_ = _enemy;
}



void EnemyHPRenderer::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale = vm->GetValue<Vec3>(groupName, "scale");
	pTransform_->rotate = vm->GetValue<Vec3>(groupName, "rotate");
	uvAnchor_ = vm->GetValue<Vec2>(groupName, "uvAnchor");

	maxScale_ = GetScale();
}


void EnemyHPRenderer::SetVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->SetValue(groupName, "position", pTransform_->position);
	vm->SetValue(groupName, "scale", pTransform_->scale);
	vm->SetValue(groupName, "rotate", pTransform_->rotate);
	vm->SetValue(groupName, "uvAnchor", uvAnchor_);
}