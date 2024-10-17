#include "TitleEarth.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "CustomComponents/EarthRenderer/EarthRenderer.h"


void TitleEarth::Initialize() {

	auto earthRenderer = AddComponent<EarthRenderer>();
	earthRenderer->SetEarthTransform(pTransform_);

	pTransform_->scale = Vec3::kOne * 11.0f;

}

void TitleEarth::Update() {}

void TitleEarth::Debug() {}
