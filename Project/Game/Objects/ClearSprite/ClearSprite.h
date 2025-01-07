#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ClearSprite : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ClearSprite();
	~ClearSprite();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
