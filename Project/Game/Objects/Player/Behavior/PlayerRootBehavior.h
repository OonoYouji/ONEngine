#pragma once
#include "IPlayerBehavior.h"

struct Player::WorkRootBehavior;

class PlayerRootBehavior : public IPlayerBehavior{
public:

	PlayerRootBehavior(Player* _host);
	~PlayerRootBehavior(){}

	void Update()override;
	void InputNextBehavior();

protected:

	const Player::WorkRootBehavior& workInBehavior_;

	Vector2 direction_;
	Vector2 lastDir_;
};