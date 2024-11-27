#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultGameClearTimeRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultGameClearTimeRenderer(float _timesecond);
	~ResultGameClearTimeRenderer();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	float time_;

	class MinuteRenderer* minuteRenderer_ = nullptr;
	class SecondRenderer* secondRenderer_ = nullptr;

};
