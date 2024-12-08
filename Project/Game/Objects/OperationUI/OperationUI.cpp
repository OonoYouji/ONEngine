#include "OperationUI.h"

#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

OperationUI::OperationUI() {
	CreateTag(this);
}

OperationUI::~OperationUI() {}

void OperationUI::Initialize() {
	renderer_ = AddComponent<SpriteRenderer>();
	renderer_->SetTexture("operationUI.png");
	
	drawLayerId = 1;

	pTransform_->scale = { 0.7f, 0.4f, 1.0f };

}

void OperationUI::Update() {

}

