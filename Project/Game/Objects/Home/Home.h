#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Home : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Home();
	~Home();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

};
