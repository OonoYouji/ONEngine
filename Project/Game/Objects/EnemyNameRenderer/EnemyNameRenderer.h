#pragma once

#include "GameObjectManager/BaseGameObject.h"

class EnemyNameRenderer : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyNameRenderer();
	~EnemyNameRenderer();

	void Initialize() override;
	void Update()     override;

	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};
