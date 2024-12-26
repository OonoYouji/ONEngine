#pragma once

/// std
#include <array>

/// engine
#include "GameObjectManager/BaseGameObject.h"

class EnemyHPRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyHPRenderer(class Enemy* _enemyPtr);
	~EnemyHPRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class EnemyHPBarGauge* hpBarGauge_ = nullptr;
	class EnemyHPBarFrame* hpBarFrame_ = nullptr;
	
	/// other class pointer
	class Enemy*      pEnemy_      = nullptr;
	class BaseCamera* pMainCamera_ = nullptr;

};
