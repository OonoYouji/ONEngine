#pragma once

#include "GameObjectManager/BaseGameObject.h"

class SecondRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SecondRenderer(float _second);
	~SecondRenderer();

	void Initialize() override;
	void Update()     override;
	void Debug()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float timeSecond_;

	class NumberRenderer* renderer_ = nullptr;
};
