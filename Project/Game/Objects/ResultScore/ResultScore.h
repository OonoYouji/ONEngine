#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultScore : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultScore();
	~ResultScore();

	void Initialize() override;
	void Update()     override;


	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class NumberRenderer* numberRenderer_ = nullptr;

};
