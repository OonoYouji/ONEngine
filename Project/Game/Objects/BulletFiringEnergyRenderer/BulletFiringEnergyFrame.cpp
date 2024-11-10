#include "BulletFiringEnergyFrame.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


BulletFiringEnergyFrame::BulletFiringEnergyFrame() {
	CreateTag(this);
}

BulletFiringEnergyFrame::~BulletFiringEnergyFrame() {}

void BulletFiringEnergyFrame::Initialize() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("BulletFiringEnergyFrame.png");

}

void BulletFiringEnergyFrame::Update() {

}

