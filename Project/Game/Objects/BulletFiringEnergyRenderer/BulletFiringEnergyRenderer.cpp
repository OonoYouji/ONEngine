#include "BulletFiringEnergyRenderer.h"

/// engine
#include "VariableManager/VariableManager.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

/// objects
#include "BulletFiringEnergyFrame.h"
#include "BulletFiringEnergyGauge.h"
#include "Objects/Player/Player.h"


BulletFiringEnergyRenderer::BulletFiringEnergyRenderer() {
	CreateTag(this);
}

BulletFiringEnergyRenderer::~BulletFiringEnergyRenderer() {}

void BulletFiringEnergyRenderer::Initialize() {

	/// 描画レイヤー
	drawLayerId = SCENE_GAME_LAYER_UI;


	/// 生成、初期化、親子付け
	frame_ = new BulletFiringEnergyFrame();
	gauge_ = new BulletFiringEnergyGauge();

	frame_->Initialize();
	gauge_->Initialize();

	frame_->SetParent(pTransform_);
	gauge_->SetParent(pTransform_);

	frame_->drawLayerId = drawLayerId;
	gauge_->drawLayerId = drawLayerId;


	/// ---------------------------------------------------
	/// 外部に値を保存、読み込み
	/// ---------------------------------------------------

	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "position", pTransform_->position);
	vm->AddValue(groupName, "scale", pTransform_->scale);

	vm->LoadSpecificGroupsToJson("./Resources/Parameters/Objects", groupName);

	ApplyVariables();

}

void BulletFiringEnergyRenderer::Update() {

	ApplyVariables();
	pTransform_->scale.x = pTransform_->scale.y / 8.0f;
}

void BulletFiringEnergyRenderer::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	pTransform_->position = vm->GetValue<Vec3>(groupName, "position");
	pTransform_->scale    = vm->GetValue<Vec3>(groupName, "scale");

}

void BulletFiringEnergyRenderer::SetPlayer(Player* _player) {
	pPlayer_ = _player;

	gauge_->SetPlayer(_player);
}

