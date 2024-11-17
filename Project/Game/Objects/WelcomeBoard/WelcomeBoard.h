#pragma once

#include "GameObjectManager/BaseGameObject.h"

class WelcomeBoard : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	WelcomeBoard();
	~WelcomeBoard();

	void Initialize() override;
	void Update()     override;


	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


};
