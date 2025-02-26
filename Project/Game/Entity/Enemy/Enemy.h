#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// ===================================================
/// 敵のクラス
/// ===================================================
class Enemy : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;


};

