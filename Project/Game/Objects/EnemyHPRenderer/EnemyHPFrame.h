#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyHPFrame : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyHPFrame();
	~EnemyHPFrame();

	void Initialize() override;
	void Update()     override;

	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
