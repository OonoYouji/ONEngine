#pragma once

#include "BasePlayerBehavior.h"

class PlayerJumpBehavior : public BasePlayerBehavior {
public:

	PlayerJumpBehavior(class Player* _player);
	~PlayerJumpBehavior();

	void Start()  override;
	void Update() override;
	void Exit()   override;

	std::string GetNextBehavior() override;
	bool CanExit() override;

private:

};