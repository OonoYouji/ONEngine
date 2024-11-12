#include "Rock.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Rock::Rock() {
	CreateTag(this);
}

Rock::~Rock() {}

void Rock::Initialize() {

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Rock");

	pTransform_->position = { 21.8f, 11.6f, 82.6f };
	pTransform_->scale = Vec3::kOne * 2.0f;

}

void Rock::Update() {

}

