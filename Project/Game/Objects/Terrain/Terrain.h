#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// ///////////////////////////////////////////////////
/// 地形のオブジェクトクラス
/// ///////////////////////////////////////////////////
class Terrain : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Terrain();
	~Terrain();

	void Initialize() override;
	void Update()     override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================



};

