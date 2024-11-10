#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BulletFiringEnergyRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BulletFiringEnergyRenderer();
	~BulletFiringEnergyRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	/// frame, gauge
	class BulletFiringEnergyFrame* frame_ = nullptr;
	class BulletFiringEnergyGauge* gauge_ = nullptr;

};
