#include "TitleEarth.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

void TitleEarth::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("Sphere");

}

void TitleEarth::Update() {}

void TitleEarth::Debug() {}
