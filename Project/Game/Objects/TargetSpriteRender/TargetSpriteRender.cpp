#include "TargetSpriteRender.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// user
#include "Scenes/Scene_Game.h"
#include "Objects/Camera/GameCamera.h"


TargetSpriteRender::TargetSpriteRender(GameCamera* _gameCamera)
	: pGameCamera_(_gameCamera) {
	CreateTag(this);
}

TargetSpriteRender::~TargetSpriteRender() {}

void TargetSpriteRender::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Target");
	drawLayerId = GAME_SCENE_LAYER_TARGET_SPRITE;
}

void TargetSpriteRender::Update() {

	if(pGameCamera_) {
		/// ビルボード計算
		pTransform_->rotate = pGameCamera_->GetTransform()->rotate;
		pTransform_->scale = pGameCamera_->GetTransform()->scale;
	}

}

