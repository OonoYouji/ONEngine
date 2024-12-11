#include "PlayerMotionDebugRenderer.h"

/// engine
#include "ComponentManager/MeshInstancingRenderer/MeshInstancingRenderer.h"
#include "ComponentManager/SplinePathRenderer/SplinePathRenderer.h"

PlayerMotionDebugRenderer::PlayerMotionDebugRenderer(Player* _player) {
	CreateTag(this);
}

PlayerMotionDebugRenderer::~PlayerMotionDebugRenderer() {}

void PlayerMotionDebugRenderer::Initialize() {

	splinePathRenderer_ = AddComponent<SplinePathRenderer>(6);
	meshInstancingRenderer_ = AddComponent<MeshInstancingRenderer>(32);
	meshInstancingRenderer_->SetModel("Sphere");
}

void PlayerMotionDebugRenderer::Update() {

}

