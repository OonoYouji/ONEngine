#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Explostion : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Explostion();
	~Explostion();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;
	class GameCamera*   pGameCamera_  = nullptr;

	float lifeTime_    = 2.0f;
	float currentTime_ = 0.0f;
};
