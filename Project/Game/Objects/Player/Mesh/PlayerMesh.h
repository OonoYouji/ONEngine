#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerMesh : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMesh();
	~PlayerMesh();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// components
	class MeshRenderer* meshRenderer_ = nullptr;
};
