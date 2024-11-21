#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleStartText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleStartText();
	~TitleStartText();

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
