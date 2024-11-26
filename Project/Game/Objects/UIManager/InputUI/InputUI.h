#pragma once

#include "GameObjectManager/BaseGameObject.h"

class InputUI : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	InputUI();
	~InputUI();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
