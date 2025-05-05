#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// DirectionalLightObject
/// ////////////////////////////////////////////////
class DirectionalLightObject : public IEntity {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	DirectionalLightObject() = default;
	~DirectionalLightObject() = default;

	void Initialize() override;
	void Update() override;

};

