#include "Ground.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Ground::Ground() {
	CreateTag(this);
}

Ground::~Ground() {}

void Ground::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(GetTag());
}

void Ground::Update() {

}

