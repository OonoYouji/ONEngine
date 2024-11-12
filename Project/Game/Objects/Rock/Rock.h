#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Rock : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Rock();
	~Rock();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
