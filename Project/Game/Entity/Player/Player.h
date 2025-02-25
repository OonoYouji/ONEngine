#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"
#include "Engine/Core/Utility/Math/Vector2.h"

/// ===================================================
/// Player
/// ===================================================
class Player : public IEntity {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	Player();
	~Player();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	Vector2 velocity_;
	float   speed_;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================



};

