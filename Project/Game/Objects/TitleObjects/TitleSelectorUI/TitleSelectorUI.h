#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleSelectorUI : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleSelectorUI();
	~TitleSelectorUI();

	void Initialize() override;
	void Update()     override;


	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	class SpriteRenderer* spriteRenderer_ = nullptr;

	Vec3 keyboardUIScale_, gamepadUIScale_;

};
