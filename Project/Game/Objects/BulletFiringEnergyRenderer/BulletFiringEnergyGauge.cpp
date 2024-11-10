#include "BulletFiringEnergyGauge.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


BulletFiringEnergyGauge::BulletFiringEnergyGauge() {
	CreateTag(this);
}

BulletFiringEnergyGauge::~BulletFiringEnergyGauge() {}

void BulletFiringEnergyGauge::Initialize() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("BulletFiringEnergyGauge.png");

}

void BulletFiringEnergyGauge::Update() {

}

