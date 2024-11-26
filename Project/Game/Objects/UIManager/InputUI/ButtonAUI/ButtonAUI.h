#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ButtonAUI : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ButtonAUI();
	~ButtonAUI();

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
