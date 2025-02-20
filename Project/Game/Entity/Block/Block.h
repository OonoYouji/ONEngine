#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// user
#include "../Config/EntityConfig.h"

/// ==================================================================================
/// Block クラス
/// ==================================================================================
class Block : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Block();
	~Block();

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

