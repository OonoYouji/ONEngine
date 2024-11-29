#pragma once

#include "BasePlayerBehavior.h"

class PlayerRootBehavior : public BasePlayerBehavior {
public:

	PlayerRootBehavior(class Player* _player);
	~PlayerRootBehavior();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	std::string GetNextBehavior() override;

	bool CanExit() override;

private:
};