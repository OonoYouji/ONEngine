#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BaseBackgroundObject : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseBackgroundObject();
	~BaseBackgroundObject();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
