#include "BulletFiringEnergyRenderer.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

/// objects
#include "BulletFiringEnergyFrame.h"
#include "BulletFiringEnergyGauge.h"


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

}

void BulletFiringEnergyRenderer::Update() {

}

