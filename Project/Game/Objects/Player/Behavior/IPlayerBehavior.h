#pragma once

#include "Input/Gamepad.h"
#include "Input/Keyboard.h"

enum class InAttackBehaviorPhase{
	Anticipation,
	Charge,
	Attack,
	Recovery
};

class Player;
class IPlayerBehavior{
public:
	IPlayerBehavior(Player* _host):host_(_host){
	/*	controlKeys_  = {
			.moveFront_ = static_cast<WORD>(KeyCode::W),
			.moveBack_ = static_cast<WORD>(KeyCode::S),
			.moveLeft_ = static_cast<WORD>(KeyCode::A),
			.moveRight_ = static_cast<WORD>(KeyCode::D),

			.avoidance_ = static_cast<WORD>(KeyCode::LeftShift),
			.weakAttack_ = static_cast<WORD>(KeyCode::Space),
			.hardAttack_ = static_cast<WORD>(KeyCode::F)
		};
		controlPadButtons_  = {
				.moveFront_ = static_cast<WORD>(PadCode::Up),
				.moveBack_ = static_cast<WORD>(PadCode::Down),
				.moveLeft_ = static_cast<WORD>(PadCode::Left),
				.moveRight_ = static_cast<WORD>(PadCode::Right),

				.avoidance_ = static_cast<WORD>(PadCode::A),
				.weakAttack_ = static_cast<WORD>(PadCode::Y),
				.hardAttack_ = static_cast<WORD>(PadCode::B)
		};*/
	}
	virtual ~IPlayerBehavior(){}

	virtual void Update() = 0;
protected:
	Player* host_;

	float anticipationTime_;
	float chargeTime_;
	float attackTime_;
	float RecoveryTime_;

};
