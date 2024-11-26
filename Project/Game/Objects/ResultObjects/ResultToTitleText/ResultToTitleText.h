#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultToTitleText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultToTitleText();
	~ResultToTitleText();

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
