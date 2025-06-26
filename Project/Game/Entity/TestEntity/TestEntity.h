#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// TestEntity
/// ////////////////////////////////////////////////
class TestEntity : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	TestEntity() = default;
	~TestEntity() override = default;

	void Initialize() override;
	void Update() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

};

