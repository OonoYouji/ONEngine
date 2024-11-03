#pragma once
#include "IPlayerBehavior.h"

class PlayerRootBehavior :
    public IPlayerBehavior{
public:
	PlayerRootBehavior(Player* _host):IPlayerBehavior(_host){}
	 ~PlayerRootBehavior(){}

	void Update()override;
protected:
	Vector2 direction_;
	Vector2 lastDir_;
};