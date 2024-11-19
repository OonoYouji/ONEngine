#include "SkyDome.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

SkyDome::SkyDome() {
	CreateTag(this);
}

SkyDome::~SkyDome() {}

void SkyDome::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("SkyDome");
}

void SkyDome::Update() {

}

