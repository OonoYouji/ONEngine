#include "TargetSpriteRender.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// user
#include "Scenes/Scene_Game.h"


TargetSpriteRender::TargetSpriteRender() {
	CreateTag(this);
}

TargetSpriteRender::~TargetSpriteRender() {}

void TargetSpriteRender::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Target");
	drawLayerId = GAME_SCENE_LAYER_TARGET_SPRITE;
}

void TargetSpriteRender::Update() {

}

