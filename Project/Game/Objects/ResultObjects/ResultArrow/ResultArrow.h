#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultArrow : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultArrow();
	~ResultArrow();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;
};
