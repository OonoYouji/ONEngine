#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleExitText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleExitText();
	~TitleExitText();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};