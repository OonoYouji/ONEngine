#pragma once

#include "BasePlayerBehavior.h"

class PlayerDushBehavior : BasePlayerBehavior {
public:

	PlayerDushBehavior(class Player* _player);
	~PlayerDushBehavior();

	void Initialize() override;
	void Update() override;
	void Finalize() override;

private:

};