#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ButtonLTUI : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ButtonLTUI();
	~ButtonLTUI();

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
