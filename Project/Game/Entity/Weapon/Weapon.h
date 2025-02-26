#pragma once

/// engine
#include "Engine/Entity/Interface/IEntity.h"

/// ===================================================
/// 武器のクラス
/// ===================================================
class Weapon final : public IEntity {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Weapon();
	~Weapon();

	void Initialize() override;
	void Update()     override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;

};

