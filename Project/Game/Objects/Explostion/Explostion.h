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
	class ExplostionParticle* particle_ = nullptr;

	float lifeTime_    = 2.0f;
	float currentTime_ = 0.0f;
};


class ExplostionParticle : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ExplostionParticle();
	~ExplostionParticle();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;
};
