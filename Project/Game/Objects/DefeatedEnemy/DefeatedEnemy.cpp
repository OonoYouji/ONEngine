#include "DefeatedEnemy.h"

/// std
#include <numbers>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// scenes
#include "Scenes/Scene_Game.h"

DefeatedEnemy::DefeatedEnemy() {
	CreateTag(this);
}

DefeatedEnemy::~DefeatedEnemy() {}

void DefeatedEnemy::Initialize() {

	drawLayerId = SCENE_GAME_LAYER_DEFEATED_ENEMY;

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Sphere");

}

void DefeatedEnemy::Update() {
	pTransform_->rotate.y += std::fmod(pTransform_->rotate.y, 2.0f * std::numbers::pi_v<float>);
}

