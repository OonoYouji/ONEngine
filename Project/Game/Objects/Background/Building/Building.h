#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Building : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Building();
	~Building();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

};
