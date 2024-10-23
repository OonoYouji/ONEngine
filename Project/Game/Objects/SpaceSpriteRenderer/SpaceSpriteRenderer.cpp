#include "SpaceSpriteRenderer.h"

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


SpaceSpriteRenderer::SpaceSpriteRenderer() {
	CreateTag(this);
}

SpaceSpriteRenderer::~SpaceSpriteRenderer() {}

void SpaceSpriteRenderer::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("space.png");

}

void SpaceSpriteRenderer::Update() {

}
