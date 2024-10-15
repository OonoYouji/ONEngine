#include "TitleEarth.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

void TitleEarth::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Sphere");

	pTransform_->scale = Vec3::kOne * 11.0f;

}

void TitleEarth::Update() {}

void TitleEarth::Debug() {}
