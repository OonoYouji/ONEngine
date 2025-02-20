#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

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


private:

	/// ===================================================
	/// private : methods
	/// ===================================================


};

