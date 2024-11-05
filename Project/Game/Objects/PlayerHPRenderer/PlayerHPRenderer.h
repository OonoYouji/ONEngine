#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerHPRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerHPRenderer();
	~PlayerHPRenderer();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void ApplyVariables();
	void SetVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
