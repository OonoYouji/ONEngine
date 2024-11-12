#pragma once

#include "GameObjectManager/BaseGameObject.h"

class DefeatedEnemy : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DefeatedEnemy();
	~DefeatedEnemy();

	void Initialize() override;
	void Update()     override;

	void ApplyVariables();

	void StartRotate();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

	float activeLifeTime_ = 0.0f;

};
