#pragma once
#include"Objects/PlayerBehavior/BasePlayerBehavior.h"

class PlayerPowerUp : public BasePlayerBehavior {
public:
	//コンストラクタ
	PlayerPowerUp(Player* player);
	~PlayerPowerUp();
	void Update()override;

private:
	bool isCameraZoomOut_ = false;
};
