#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyHPBarGauge : public BaseGameObject {
	friend class EnemyHPRenderer;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyHPBarGauge();
	~EnemyHPBarGauge();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
