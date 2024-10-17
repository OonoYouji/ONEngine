#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"

class GameManager : public BaseGameObject {
public:

	GameManager();
	~GameManager();

	void Initialize() override;
	void Update()     override;
	void LastUpdate() override;
	void Debug()      override;


	void SetInGameObjectArray(const std::vector<BaseGameObject*>& _inGameObjectArray);

private:

	/// ゲームが開始しているかのフラグ
	bool isGameStarted_    = false;
	bool preIsGameStarted_ = false;

	std::vector<BaseGameObject*> inGameObjectArray_;

};
