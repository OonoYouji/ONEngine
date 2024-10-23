#include "SelectedFrame.h"


/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


SelectedFrame::SelectedFrame() {
	CreateTag(this);
}

SelectedFrame::~SelectedFrame() {}

void SelectedFrame::Initialize() {
	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("resultFleam.png");

	pTransform_->position.y = -0.5f;

	pTransform_->scale = { 1.3f, 0.5f, 1.0f };
}

void SelectedFrame::Update() {

}
