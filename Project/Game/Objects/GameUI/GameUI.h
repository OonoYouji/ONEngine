#pragma once

#include "GameObjectManager/GameObjectManager.h"

class GameUI : public BaseGameObject {
public:

	GameUI();
	~GameUI();

	void Initialize() override;
	void Update()     override;

	void SetPlayer(class Player* _player);
	void SetBoss(class Boss* _boss);

private:
	class PowerUpGauge* powerUpGauge_ = nullptr;
	class BossHP* bossHP_ = nullptr;
};