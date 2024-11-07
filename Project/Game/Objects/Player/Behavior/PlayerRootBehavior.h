#pragma once
#include "IPlayerBehavior.h"

class Player::WorkRootBehavior;
class PlayerRootBehavior :
    public IPlayerBehavior{
public:
	PlayerRootBehavior(Player* _host);
	 ~PlayerRootBehavior(){}

	void Update()override;
protected:
	const Player::WorkRootBehavior& workInBehavior_;

	Vector2 direction_;
	Vector2 lastDir_;
};