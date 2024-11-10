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


	/// <summary>
	/// json保存の値を適用する
	/// </summary>
	void ApplyVariables();


	/// ===================================================
	/// public : setter - other class pointer
	/// ===================================================

	void SetPlayer(class Player* _player);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// frame, gauge
	class BulletFiringEnergyFrame* frame_ = nullptr;
	class BulletFiringEnergyGauge* gauge_ = nullptr;

	
	/// other class pointer
	class Player* pPlayer_ = nullptr;


};
