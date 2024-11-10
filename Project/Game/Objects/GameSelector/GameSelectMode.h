#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// objects
#include "GameSelector.h"

class GameSelectMode : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameSelectMode();
	~GameSelectMode();

	void Initialize() override;
	void Update()     override;

	template<GAME_SELECT_MODE T>
	void ThisInitialize();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};

