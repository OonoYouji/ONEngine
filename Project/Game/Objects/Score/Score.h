#pragma once

#include "GameObjectManager/BaseGameObject.h"

class Score : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Score();
	~Score();

	void Initialize() override;
	void Update()     override;


	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class NumberRenderer* numberRenderer_ = nullptr;
};
