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


	/// ===================================================
	/// public : setter - other class pointer
	/// ===================================================

	void SetPlayer(class Player* _player);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// component
	class SpriteRenderer* spriteRenderer_ = nullptr;


	/// other class pointer
	class Player* pPlayer_ = nullptr;

};
