#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// SourceEntity
/// ////////////////////////////////////////////////
class SourceEntity : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	SourceEntity() = default;
	~SourceEntity() override = default;

	void Initialize() override;
	void Update() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

};

