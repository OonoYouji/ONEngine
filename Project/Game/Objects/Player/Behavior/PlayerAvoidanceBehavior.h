#pragma once
#include "IPlayerBehavior.h"
#include "../Player.h"

#include <functional>

class Vector3;

class PlayerAvoidanceBehavior :
	public IPlayerBehavior{
public:
	PlayerAvoidanceBehavior(Player* _host);
	~PlayerAvoidanceBehavior(){}

	void Update()override;
private:
	std::function<void()> currentUpdate_;
	void StartupUpdate();
	void Avoidance();
	void EndLagUpdate();
private:
	float currentTime_;

	Vector3 beforePos_;
	Vector3 afterPos_;
	const Player::WorkAvoidanceBehavior& workInBehavior_;
};