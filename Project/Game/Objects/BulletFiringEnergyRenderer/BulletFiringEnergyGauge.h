#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BulletFiringEnergyGauge : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BulletFiringEnergyGauge();
	~BulletFiringEnergyGauge();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;
};
