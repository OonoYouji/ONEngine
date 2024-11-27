#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameOverToTitleText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameOverToTitleText();
	~GameOverToTitleText();

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
