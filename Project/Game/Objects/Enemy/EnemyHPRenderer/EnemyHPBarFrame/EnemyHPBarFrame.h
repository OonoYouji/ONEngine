#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyHPBarFrame : public BaseGameObject {
	friend class EnemyHPRenderer;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyHPBarFrame();
	~EnemyHPBarFrame();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
