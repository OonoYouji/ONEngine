#include "ClearSprite.h"

#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

ClearSprite::ClearSprite() {
	CreateTag(this);
}

ClearSprite::~ClearSprite() {}

void ClearSprite::Initialize() {

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("Clear.png");

	pTransform_->scale = { 8.0f, 4.0f, 1.0f };

}

void ClearSprite::Update() {

}

