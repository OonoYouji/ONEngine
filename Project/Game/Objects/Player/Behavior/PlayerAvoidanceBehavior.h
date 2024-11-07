#pragma once
#include "IPlayerBehavior.h"

#include <functional>

class Player::WorkAvoidanceBehavior;
class PlayerAvoidanceBehavior :
	public IPlayerBehavior{
public:
	PlayerAvoidanceBehavior(Player* _host,const Vector2& _direction);
	~PlayerAvoidanceBehavior(){}

	void Update()override;
private:
	std::function<void ()> currentUpdate_;
	void StartupUpdate();
	void Avoidance();
	void EndLagUpdate();
private:
	float maxTime_;
	float currentTime_;

	Vector2 direction_;

	Vector3 beforePos_;
	Vector3 afterPos_;

	const Player::WorkAvoidanceBehavior& workInBehavior_;
};