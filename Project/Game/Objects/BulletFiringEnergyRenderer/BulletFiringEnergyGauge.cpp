#include "BulletFiringEnergyGauge.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// objects
#include "Objects/Player/Player.h"


BulletFiringEnergyGauge::BulletFiringEnergyGauge() {
	CreateTag(this);
}

BulletFiringEnergyGauge::~BulletFiringEnergyGauge() {}

void BulletFiringEnergyGauge::Initialize() {
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("BulletFiringEnergyGauge.png");

	spriteRenderer_->SetAnchor(Vec2(0.5f, 1.0f));

	pTransform_->position = {
		0.0f, -0.5f, 5.0f
	};

}

void BulletFiringEnergyGauge::Update() {

	/// 線形補完用に変換
	float firingGaugeT = pPlayer_->GetFiringGauge() / 100.0f;

	pTransform_->scale.y = std::lerp(
		0.0f, 1.0f, firingGaugeT
	);

}

void BulletFiringEnergyGauge::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}

