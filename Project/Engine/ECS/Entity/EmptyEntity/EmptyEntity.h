#pragma once

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// //////////////////////////////////////////////////
/// 空のオブジェクト
/// //////////////////////////////////////////////////
class EmptyEntity : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	EmptyEntity() = default;
	~EmptyEntity() = default;

	void Initialize() override;
	void Update() override;
};

