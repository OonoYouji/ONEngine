#pragma once

#include "GameObjectManager/BaseGameObject.h"

class SkyDome : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SkyDome();
	~SkyDome();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
