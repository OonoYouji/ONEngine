#pragma once

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

/// ////////////////////////////////////////////////
/// Puzzle
/// ////////////////////////////////////////////////
class Puzzle : public IEntity {
public:
	/// =========================================
	/// public : methods
	/// =========================================
	
	Puzzle() = default;
	~Puzzle() override = default;

	void Initialize() override;
	void Update() override;

private:
	/// =========================================
	/// private : objects
	/// =========================================

	class Player* player_ = nullptr;

};

