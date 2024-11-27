#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BGMObj : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BGMObj();
	~BGMObj();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
