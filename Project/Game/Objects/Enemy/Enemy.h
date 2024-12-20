#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Enemy : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
