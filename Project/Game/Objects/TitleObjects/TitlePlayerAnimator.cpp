#include "TitlePlayerAnimator.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


void TitlePlayerAnimator::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");



}

void TitlePlayerAnimator::Update() {

}

void TitlePlayerAnimator::Debug() {

}
