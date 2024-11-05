#include "PlayerHPRenderer.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

PlayerHPRenderer::PlayerHPRenderer() {
	CreateTag(this);
}

PlayerHPRenderer::~PlayerHPRenderer() {}

void PlayerHPRenderer::Initialize() {

	/// ui layerに表示
	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("white2x2");
	spriteRenderer_->SetUVSize(Vec2(0.5f, 0.5f));
}

void PlayerHPRenderer::Update() {

}

