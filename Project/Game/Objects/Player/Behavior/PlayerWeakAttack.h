#pragma once
#include "IPlayerBehavior.h"
#include "../Player.h"

#include <functional>
#include <memory>
#include <stdint.h>

class PlayerWeakAttack :
	public IPlayerBehavior{
public:
	PlayerWeakAttack(Player* player,int32_t comboNum);
	~PlayerWeakAttack(){}

	void Update()override;
private:
	std::function<void()> currentUpdate_;
	void StartupUpdate();
	void WeakAttack();
	void EndLagUpdate();
private:
	std::unique_ptr<IPlayerBehavior> nextBehavior_;
	int32_t comboNum_;

	float currentTime_;

	const Player::WorkWeakAttackBehavior& workInBehavior_;
	Vector2 direction_;
};