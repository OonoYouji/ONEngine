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

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};