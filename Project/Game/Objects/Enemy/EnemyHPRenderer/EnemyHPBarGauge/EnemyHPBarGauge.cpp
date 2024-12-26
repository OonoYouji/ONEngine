#include "EnemyHPBarGauge.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

EnemyHPBarGauge::EnemyHPBarGauge() {
	CreateTag(this);
}

EnemyHPBarGauge::~EnemyHPBarGauge() {}

void EnemyHPBarGauge::Initialize() {

	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("EnemyHPBarGauge");
	meshRenderer_->SetMaterial("EnemyHPBarGauge.png");
	meshRenderer_->SetIsLighting(false);

}

void EnemyHPBarGauge::Update() {

}

