#include "Reticle.h"

/// std
#include <cassert>

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Camera/GameCamera.h"


Reticle::Reticle() {
	CreateTag(this);
}

Reticle::~Reticle() {}

void Reticle::Initialize() {

	/// component
	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");

	/// transform
	pTransform_->position = { 0, 0, 10.0f };
	pTransform_->scale *= 0.5f;
}

void Reticle::Update() {

}



void Reticle::SetGameCamera(GameCamera* _gameCamera) {
	pGameCamera_ = _gameCamera;
	assert(pGameCamera_);

	SetParent(pGameCamera_->GetTransform());

}
