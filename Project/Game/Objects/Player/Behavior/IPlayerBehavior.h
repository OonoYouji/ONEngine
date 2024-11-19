#pragma once

#include "Input/Gamepad.h"
#include "Input/Keyboard.h"

enum class InAttackBehaviorPhase {
	Anticipation,
	Charge,
	Attack,
	Recovery
};

class Player;
class IPlayerBehavior {
public:

	IPlayerBehavior(Player* _host) : host_(_host) {}
	virtual ~IPlayerBehavior() {}

	virtual void Update() = 0;
protected:
	Player* host_;
};
