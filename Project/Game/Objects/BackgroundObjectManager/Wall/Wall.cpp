#include "Wall.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Wall::Wall() {
	CreateTag(this);
}

Wall::~Wall() {}

void Wall::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(GetTag());
}

void Wall::Update() {

}

