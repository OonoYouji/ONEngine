#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// ///////////////////////////////////////////////////
/// Player
/// ///////////////////////////////////////////////////
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

	Vector3 velocity_;
	float   speed_;

	int     type_; ///< プレイヤーのタイプ

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================



};

