#pragma once

#include "GameObjectManager/BaseGameObject.h"

class MinuteRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	MinuteRenderer(float _minute);
	~MinuteRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float minute_;

};
