#include "SkyDome.h"

/// externals
#include <imgui.h>

/// compoent
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


SkyDome::SkyDome() {
	CreateTag(this);
}

SkyDome::~SkyDome() {}

void SkyDome::Initialize() {
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("SkyDome");
	meshRenderer_->SetMaterial("SkyDome.png");
	meshRenderer_->SetUVScale(uvScale_);

	pTransform_->scale = Vec3::kOne * 10.0f;
}

void SkyDome::Update() {
	pTransform_->position = pOffsetObject_->GetPosition();
}

void SkyDome::Debug() {
	if(ImGui::DragFloat2("uv scale", &uvScale_.x, 0.1f)) {
		meshRenderer_->SetUVScale(uvScale_);
	}
}

void SkyDome::SetOffsetObject(BaseGameObject* _offsetObject) {
	pOffsetObject_ = _offsetObject;
}

