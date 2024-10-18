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
	spriteRenderer_->SetTexture("white2x2.png");
}

void SelectedFrame::Update() {

}
