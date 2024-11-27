#include "Stage.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"


Stage::Stage() {
	CreateTag(this);
}

Stage::~Stage() {}

void Stage::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Stage");

}

void Stage::Update() {

}

