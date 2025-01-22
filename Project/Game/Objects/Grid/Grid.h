#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Grid : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Grid();
	~Grid();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* renderer_ = nullptr;

};
