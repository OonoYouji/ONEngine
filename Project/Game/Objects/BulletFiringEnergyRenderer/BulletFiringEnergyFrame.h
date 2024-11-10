#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BulletFiringEnergyFrame : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BulletFiringEnergyFrame();
	~BulletFiringEnergyFrame();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;
};
