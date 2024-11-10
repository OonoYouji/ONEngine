#pragma once

#include "GameObjectManager/BaseGameObject.h"

enum GAME_SELECT_MODE {
	GAME_SELECT_MODE_START,
	GAME_SELECT_MODE_EXIT,
	GAME_SELECT_MODE_COUNT,
};


class GameSelector : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameSelector();
	~GameSelector();

	void Initialize() override;
	void Update()     override;


	int GetGameSelectMode() const { return gameSelectMode_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	int gameSelectMode_ = 0;

	std::array<class GameSelectMode*, GAME_SELECT_MODE_COUNT> gameSelectModeArray_;

};
