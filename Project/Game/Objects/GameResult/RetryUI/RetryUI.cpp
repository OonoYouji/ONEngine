#include "RetryUI.h"

/// componetns
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


RetryUI::RetryUI() {
    CreateTag(this);
}

RetryUI::~RetryUI() {}

void RetryUI::Initialize() {

	drawLayerId = 1;

	/// component
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("resultRetry.png");

	/// transform
	pTransform_->position.x = -1.0f;
	pTransform_->position.y = -0.5f;

	pTransform_->scale.x = 1.8f;
}

void RetryUI::Update() {

}
