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


	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<BaseGameObject*> uis_;

};
