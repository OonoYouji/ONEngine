#include "Building.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

Building::Building() {
	CreateTag(this);
}

Building::~Building() {}

void Building::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Building");

}

void Building::Update() {

}

