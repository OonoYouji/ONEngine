#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ModelLoadChecker : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ModelLoadChecker();
	~ModelLoadChecker();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
