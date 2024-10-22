#include "BackTitleUI.h"

/// componetns
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


BackTitleUI::BackTitleUI() {
	CreateTag(this);
}

BackTitleUI::~BackTitleUI() {}

void BackTitleUI::Initialize() {

	drawLayerId = 1;

	/// component
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("uvChecker.png");

	/// transform
	pTransform_->position.x = 1.0f;
	pTransform_->position.y = -0.5f;
}

void BackTitleUI::Update() {

}
