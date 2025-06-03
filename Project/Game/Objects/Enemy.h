#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// Enemy
/// ////////////////////////////////////////////////
class Enemy : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	Enemy() = default;
	~Enemy() override = default;

	void Initialize() override;
	void Update() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

};

