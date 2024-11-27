#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameOverToRestartText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameOverToRestartText();
	~GameOverToRestartText();

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
