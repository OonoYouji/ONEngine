#pragma once

#include "BasePlayerBehavior.h"

class PlayerJumpBehavior : BasePlayerBehavior {
public:

	PlayerJumpBehavior(class Player* _player);
	~PlayerJumpBehavior();

	void Initialize() override;
	void Update() override;
	void Finalize() override;

private:

};