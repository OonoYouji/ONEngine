#pragma once

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

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

