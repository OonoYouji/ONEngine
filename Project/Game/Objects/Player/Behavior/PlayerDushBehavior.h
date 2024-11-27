#pragma once

#include "BasePlayerBehavior.h"

class PlayerDushBehavior : public BasePlayerBehavior {
public:

	PlayerDushBehavior(class Player* _player);
	~PlayerDushBehavior();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	std::string GetNextBehavior() override;
	bool CanExit() override;

private:
	

	float maxTime_;
	float currentTime_;
};