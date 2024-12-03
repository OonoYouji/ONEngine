#include "Skydome.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Skydome::Skydome() {
	CreateTag(this);
}

Skydome::~Skydome() {}

void Skydome::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(GetTag());
}

void Skydome::Update() {

}

