#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Wall : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Wall();
	~Wall();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
