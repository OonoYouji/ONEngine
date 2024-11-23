#include "EntityShadow.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

EntityShadow::EntityShadow() {
	CreateTag(this);
}

EntityShadow::~EntityShadow() {}

void EntityShadow::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel(GetTag());
}

void EntityShadow::Update() {

}

