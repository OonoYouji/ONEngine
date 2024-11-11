#include "Terrain.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


Terrain::Terrain() {
	CreateTag(this);
}

Terrain::~Terrain() {}

void Terrain::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Terrain");

	pTransform_->position = { 0.0f, -20.0f, 50.0f };
	pTransform_->scale    = Vec3::kOne * 2.0f;
}

void Terrain::Update() {

}

