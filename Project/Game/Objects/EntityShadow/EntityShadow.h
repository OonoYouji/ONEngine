#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EntityShadow : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EntityShadow();
	~EntityShadow();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
