#include "EnemyHPBarFrame.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"



EnemyHPBarFrame::EnemyHPBarFrame() {
	CreateTag(this);
}

EnemyHPBarFrame::~EnemyHPBarFrame() {}

void EnemyHPBarFrame::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("EnemyHPBarFrame");
	meshRenderer_->SetMaterial("EnemyHPBarFrame.png");
	meshRenderer_->SetIsLighting(false);

}

void EnemyHPBarFrame::Update() {

}

