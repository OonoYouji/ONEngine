#pragma once

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

/// ===================================================
/// グリッドクラス
/// ===================================================
class Grid final : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Grid() {}
	~Grid() {}

	void Initialize() override;
	void Update()     override;
};


/// ===================================================
/// グリッドクラス
/// ===================================================
class Grid2D final : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Grid2D() {}
	~Grid2D() {}

	void Initialize() override;
	void Update()     override;
};

