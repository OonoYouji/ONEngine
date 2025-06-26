#pragma once

/// engine
#include "../Interface/ECSISystem.h"

class CollisionSystem : public ECSISystem {
public:
	/// =======================================
	/// public : methods
	/// =======================================

	CollisionSystem() = default;
	virtual ~CollisionSystem() = default;

	// Update method to handle collision logic
	void Update(class EntityComponentSystem* _pEntityComponentSystem) override;

private:
	/// =======================================
	/// private : objects
	/// =======================================



};

