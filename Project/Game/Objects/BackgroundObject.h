#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// BackgroundObject
/// ////////////////////////////////////////////////
class BackgroundObject : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	BackgroundObject() = default;
	~BackgroundObject() override = default;

	void Initialize() override;
	void Update() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

};

