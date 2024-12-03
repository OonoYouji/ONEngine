#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Skydome : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Skydome();
	~Skydome();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
