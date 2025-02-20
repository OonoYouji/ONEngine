#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"
#include "../Config/EntityConfig.h"

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

	int type_; ///< BlockType参照


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	void SetType(int _type) { type_ = _type; }

	int GetType() const { return type_; }


};

