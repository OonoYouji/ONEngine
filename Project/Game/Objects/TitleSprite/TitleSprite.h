#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitleSprite : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	TitleSprite();
	~TitleSprite();

	void Initialize() override;
	void Update()     override;

	
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
