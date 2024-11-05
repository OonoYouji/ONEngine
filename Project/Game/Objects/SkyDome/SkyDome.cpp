#include "SkyDome.h"

/// compoent
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


SkyDome::SkyDome() {
	CreateTag(this);
}

SkyDome::~SkyDome() {}

void SkyDome::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("SkyDome");

	pTransform_->scale = Vec3::kOne * 10.0f;
}

void SkyDome::Update() {
	pTransform_->position = pOffsetObject_->GetPosition();
}

void SkyDome::SetOffsetObject(BaseGameObject* _offsetObject) {
	pOffsetObject_ = _offsetObject;
}

