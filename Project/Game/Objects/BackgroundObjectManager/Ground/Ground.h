#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Ground : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Ground();
	~Ground();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
