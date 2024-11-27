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
	Transform* parent = GetParent();
	if(parent) {
		if(parent->position.y >= 0.0f) {
			pTransform_->position.y = -parent->position.y;
		} else {
			pTransform_->position.y = 0.0f;
		}
	}
}

