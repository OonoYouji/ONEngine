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

	void AddVariables() override;
	void ApplyVariables() override;

private:
	
	Vec3 dushVelocity_;
	float dushSpeed_;

	float maxTime_;
	float currentTime_;
};