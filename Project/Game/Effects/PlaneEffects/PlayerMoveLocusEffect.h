#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// /////////////////////////////////////////////////////////////////
/// PlayerMoveLocusEffect
/// /////////////////////////////////////////////////////////////////
class PlayerMoveLocusEffect : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	PlayerMoveLocusEffect() = default;
	~PlayerMoveLocusEffect() = default;
	
	void Initialize() override;
	void Update()     override;

};

