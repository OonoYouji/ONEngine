#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// /////////////////////////////////////////////////////////////////
/// Player
/// /////////////////////////////////////////////////////////////////
class Player : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================


};

