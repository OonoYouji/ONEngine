#pragma once

#include "IPlayerBehavior.h"


class PlayerStrongAttack : public IPlayerBehavior {
public:

	PlayerStrongAttack(Player* _player);
	~PlayerStrongAttack() {}

	void Update() override;

private:

};