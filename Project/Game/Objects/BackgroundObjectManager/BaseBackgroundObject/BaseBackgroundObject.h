#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BaseBackgroundObject : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseBackgroundObject(const Vec3& wPosition);
	~BaseBackgroundObject();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
