#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerHPFrame : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerHPFrame();
	~PlayerHPFrame();

	void Initialize() override;
	void Update()     override;

	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
