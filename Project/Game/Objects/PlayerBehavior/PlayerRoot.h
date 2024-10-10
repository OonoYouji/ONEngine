#pragma once
#include"Objects/PlayerBehavior/BasePlayerBehavior.h"

class PlayerRoot : public BasePlayerBehavior {
public:
	//コンストラクタ
	PlayerRoot(Player* player);
	~PlayerRoot();
	void Update()override;
	//void showState() override;
};
